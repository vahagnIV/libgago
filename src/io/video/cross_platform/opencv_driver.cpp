//
// Created by Vahagn Yeghikyan on 2020-03-01.
//

#include "opencv_driver.h"
#include <opencv2/opencv.hpp>
namespace gago {
namespace io {
namespace video {

void OpenCvDriver::Initialize() {
  cv::VideoCapture camera;
  int device_counts = 0;
  while (true) {
    if (!camera.open(device_counts++)) {
      break;
    }
  }
  camera.release();
  for (int i = 0; i < device_counts; ++i) {
    OpenCvCamera *cam = OpenCvCamera::Create(i);
    if (cam) {
      std::string camera_uid = std::to_string(i);
      cameras_[camera_uid] = cam;
    }
  }
}

void OpenCvDriver::SetSettings(const std::vector<CameraSettings> &settings) {
  bool any_changes = false;
  for (const CameraSettings &setting: settings) {
    if (cameras_.find(setting.camera->GetUniqueId()) != cameras_.end()
        && cameras_[setting.camera->GetUniqueId()]->GetConfiguration() != setting.config) {
      any_changes = true;
      break;
    }
  }
  bool running;
  if (any_changes) {
    running = thread_;
    Stop();
  } else
    running = false;

  for (const CameraSettings &setting: settings) {
    // TODO: verify settings
    if (cameras_.find(setting.camera->GetUniqueId()) != cameras_.end()) {
      OpenCvCamera *camera = cameras_[setting.camera->GetUniqueId()];
      camera->SetConfiguration(setting.config);
    }
  }
  if (running)
    Start();

}

void OpenCvDriver::GetSettings(std::vector<CameraSettings> &out_settings) const {
  for (const std::pair<std::string, OpenCvCamera *> &camera: cameras_) {
    out_settings.push_back(CameraSettings(camera.second, camera.second->GetConfiguration()));
  }
}
std::vector<const CameraMeta *> OpenCvDriver::GetCameras() const {
  std::vector<const CameraMeta *> enabled_cameras_meta;
  for (const std::pair<std::string, OpenCvCamera *> & cam_name_cam: cameras_)
    if (cam_name_cam.second->Enabled()) {
      enabled_cameras_meta.push_back(cam_name_cam.second);
    }
  return enabled_cameras_meta;
}
void OpenCvDriver::RegisterWatcher(CameraWatcher *watcher) {
  bool running = thread_;
  Stop();
  watchers_.push_back(watcher);
  if (running)
    Start();
}
void OpenCvDriver::UnRegister(CameraWatcher *watcher) {
  bool running = thread_;
  Stop();

  if (!watchers_.empty())
    watchers_.erase(std::remove_if(watchers_.begin(),
                                   watchers_.end(),
                                   [&watcher](const CameraWatcher *ob) {
                                     return ob == watcher;
                                   }));
  if (running)
    Start();

}
void OpenCvDriver::Start() {

  if (nullptr == thread_) {
    cancelled_ = false;
    std::vector<const CameraMeta *> enabled_cameras_meta = GetCameras();
    for (int k = 0; k < watchers_.size(); ++k) {
      watchers_[k]->SetCameras(enabled_cameras_meta);
    }
    thread_ = new std::thread(&OpenCvDriver::MainThread, this);
  }
}
void OpenCvDriver::Stop() {
  cancelled_ = true;
  Join();
  delete thread_;
  thread_ = nullptr;
}
void OpenCvDriver::Join() {

  if (thread_)
    thread_->join();
}
void OpenCvDriver::CaptureThread(OpenCvCamera *camera_ptr,
                                 std::atomic_bool &capture_requested,
                                 std::atomic_bool &ready,
                                 long long &time) {
  while (!cancelled_) {
    std::unique_lock<std::mutex> lk(mutex_);
    ready = true;
    condition_variable_.wait(lk, [&] { return (bool) capture_requested; });
    ready = false;
    camera_ptr->Grab();
    capture_requested = false;
    time = std::chrono::high_resolution_clock::now().time_since_epoch().count();
  }

}
void OpenCvDriver::Notify(const std::shared_ptr<std::vector<Capture>> &captures) {
  for (CameraWatcher *watcher: watchers_)
    watcher->Notify(captures);
}
void OpenCvDriver::MainThread() {
  cancelled_ = false;

  std::vector<OpenCvCamera *> enabled_cameras;

  for (const std::pair<std::string, OpenCvCamera *> &cam_name_cam: cameras_)

    if (cam_name_cam.second->Enabled()) {
      enabled_cameras.push_back(cam_name_cam.second);
    }


  std::vector<std::thread *> enabled_camera_threads(enabled_cameras.size());
  std::vector<std::atomic_bool> capture_expected(enabled_cameras.size());
  std::vector<std::atomic_bool> ready(enabled_cameras.size());
  std::vector<long long> time(enabled_cameras.size());


  // Initialize threads and shared variables
  for (int j = 0; j < enabled_cameras.size(); ++j) {

    capture_expected[j] = false;
    ready[j] = false;
    enabled_camera_threads[j] =
        new std::thread(&OpenCvDriver::CaptureThread,
                        this,
                        enabled_cameras[j],
                        std::ref(capture_expected[j]),
                        std::ref(ready[j]),
                        std::ref(time[j]));
  }

  while (!cancelled_) {
    if (watchers_.empty()) {
      std::this_thread::sleep_for(std::chrono::microseconds(100));
      continue;
    }


    // Wait until all cameras are ready
    bool all_are_ready;
    do {
      all_are_ready = true;
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      for (int j = 0; j < enabled_cameras.size(); ++j) {
        all_are_ready = all_are_ready && ready[j];
      }
    } while (!cancelled_ && !all_are_ready);

    for (int j = 0; j < enabled_cameras.size(); ++j) {
      capture_expected[j] = true;
    }

    if (cancelled_)
      break;

    condition_variable_.notify_all();
    // Wait for captrue threads to finish

    std::shared_ptr<std::vector<Capture>> captures = std::make_shared<std::vector<Capture>>();
    for (int i = 0; i < enabled_cameras.size(); ++i) {
      Capture capture(enabled_cameras[i]);
      enabled_cameras[i]->Retieve(capture.data);
      capture.capture_date = time[i];
      captures->push_back(capture);
    }

    Notify(captures);
  }

  for (int j = 0; j < enabled_cameras.size(); ++j) {
    capture_expected[j] = true;
  }

  if (cancelled_)
    condition_variable_.notify_all();
  // Normally we should have another thread which does this

  for (int i = 0; i < enabled_cameras.size(); ++i) {
    enabled_camera_threads[i]->join();
    delete enabled_camera_threads[i];
  }

}

}
}
}