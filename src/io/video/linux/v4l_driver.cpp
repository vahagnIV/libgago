//
// Created by vahagn on 1/2/20.
//

#include "v4l_driver.h"
#include <boost/filesystem.hpp>
#include <pthread.h>
namespace gago {
namespace io {
namespace video {

V4lDriver::V4lDriver() {
}

void V4lDriver::Initialize() {
  if (!boost::filesystem::exists("/sys/class/video4linux"))
    return;
  for (boost::filesystem::directory_iterator it("/sys/class/video4linux");
       it != boost::filesystem::directory_iterator(); ++it) {
    if (boost::filesystem::is_directory(it->path())) {
      std::string device_path = "/dev/" + it->path().filename().string();
      V4lCamera * camera = V4lCamera::Create(device_path);
      if (camera)
        cameras_[camera->GetUniqueId()] = camera;
    }
  }
}

void V4lDriver::RegisterWatcher(CameraWatcher * watcher) {
  bool running = thread_;
  Stop();
  watchers_.push_back(watcher);
  if (running)
    Start();
}

void V4lDriver::UnRegister(CameraWatcher * observer) {
  bool running = thread_;
  Stop();

  if (!watchers_.empty())
    watchers_.erase(std::remove_if(watchers_.begin(),
                                   watchers_.end(),
                                   [&observer](const CameraWatcher * ob) {
                                     return ob == observer;
                                   }));
  if (running)
    Start();
}

void V4lDriver::SetSettings(const std::vector<CameraSettings> & settings) {

  bool any_changes = false;
  for (const CameraSettings & setting: settings) {
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

  for (const CameraSettings & setting: settings) {
    // TODO: verify settings
    if (cameras_.find(setting.camera->GetUniqueId()) != cameras_.end()) {
      V4lCamera * camera = cameras_[setting.camera->GetUniqueId()];
      camera->SetConfiguration(setting.config);
    }
  }
  if (running)
    Start();
}

void V4lDriver::GetSettings(std::vector<CameraSettings> & out_settings) const {
  for (const std::pair<std::string, V4lCamera *> & camera: cameras_) {
    out_settings.push_back(CameraSettings(camera.second, camera.second->GetConfiguration()));
  }
}

void V4lDriver::Start() {
  if (nullptr == thread_) {
    cancelled_ = false;
    std::vector<const CameraMeta *> enabled_cameras_meta = GetCameras();

    for (int k = 0; k < watchers_.size(); ++k) {
      watchers_[k]->SetCameras(enabled_cameras_meta);
    }
    thread_ = new std::thread(&V4lDriver::MainThread, this);
  }
}

void V4lDriver::CaptureThread(V4lCamera * camera_ptr,
                              std::atomic_bool & capture_requested,
                              std::atomic_bool & ready,
                              long long & time) {
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

void V4lDriver::MainThread() {
  cancelled_ = false;
  std::vector<V4lCamera *> enabled_cameras;
  for (const std::pair<std::string, V4lCamera *> & cam_name_cam: cameras_)
    if (cam_name_cam.second->Enabled()) {
      enabled_cameras.push_back(cam_name_cam.second);
    }

  std::vector<std::thread *> enabled_camera_threads(enabled_cameras.size());
  std::vector<std::atomic_bool> capture_expected(enabled_cameras.size());
  std::vector<std::atomic_bool> ready(enabled_cameras.size());
  std::vector<long long> time(enabled_cameras.size());


  // Initialize threads and shared variables
  for (int j = 0; j < enabled_cameras.size(); ++j) {
    enabled_cameras[j]->PrepareBuffers();
    capture_expected[j] = false;
    ready[j] = false;
    enabled_camera_threads[j] =
        new std::thread(&V4lDriver::CaptureThread,
                        this,
                        enabled_cameras[j],
                        std::ref(capture_expected[j]),
                        std::ref(ready[j]),
                        std::ref(time[j]));
  }
  std::ofstream valod("diff.txt");

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
      capture.width = enabled_cameras[i]->format_.fmt.pix.width;
      capture.height = enabled_cameras[i]->format_.fmt.pix.height;
      capture.channels = 3;
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
    enabled_cameras[i]->UnmapBuffers();
  }
}

void V4lDriver::Join() {
  if (thread_)
    thread_->join();
}

void V4lDriver::Stop() {
  cancelled_ = true;
  Join();
  delete thread_;
  thread_ = nullptr;
}

std::vector<const CameraMeta *> V4lDriver::GetCameras() const {
  std::vector<const CameraMeta *> enabled_cameras_meta;
  for (const std::pair<std::string, V4lCamera *> & cam_name_cam: cameras_)
    if (cam_name_cam.second->Enabled()) {
      enabled_cameras_meta.push_back(cam_name_cam.second);
    }
  return enabled_cameras_meta;
}

void V4lDriver::Notify(const std::shared_ptr<std::vector<Capture>> & captures) {
  for (CameraWatcher * watcher: watchers_)
    watcher->Notify(captures);
}

V4lDriver::~V4lDriver() {
  Stop();
  for (std::pair<std::string, V4lCamera *> vcam_cam: cameras_) {
    delete vcam_cam.second;
  }
}

}
}
}