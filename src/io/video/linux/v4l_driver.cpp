//
// Created by vahagn on 1/2/20.
//

#include "v4l_driver.h"
#include <boost/filesystem.hpp>
#include <pthread.h>
#include <sched.h>
namespace gago {
namespace io {
namespace video {

V4lDriver::V4lDriver() {

}

void V4lDriver::Register(algorithm::Observer<std::vector<Capture>> *observer) {
  this->observers_.push_back(observer);
}

void V4lDriver::Initialize() {
  if (!boost::filesystem::exists("/sys/class/video4linux"))
    return;
  for (boost::filesystem::directory_iterator it("/sys/class/video4linux");
       it != boost::filesystem::directory_iterator(); ++it) {
    if (boost::filesystem::is_directory(it->path())) {
      std::string device_path = "/dev/" + it->path().filename().string();
      V4lCamera *camera = V4lCamera::Create(device_path);
      if (camera)
        cameras_[camera->GetUniqueId()] = camera;
    }
  }
}

void V4lDriver::SetSettings(const std::vector<CameraSettings> & settings) {
  for (const CameraSettings & setting: settings) {
    if (cameras_.find(setting.camera->GetUniqueId()) != cameras_.end()) {
      V4lCamera *camera = cameras_[setting.camera->GetUniqueId()];
      camera->SetName(setting.config.name);
      camera->settings_.resolution_index = setting.config.resolution_index;
    }
  }
}

void V4lDriver::GetSettings(std::vector<CameraSettings> & out_settings) const {
  for (const std::pair<std::string, V4lCamera *> & camera: cameras_) {
    out_settings.push_back(CameraSettings(camera.second, camera.second->GetConfiguration()));
  }
}

void V4lDriver::Start() {
  cancelled_ = false;
  thread_ = new std::thread(&V4lDriver::MainThread, this);
}

void V4lDriver::CaptureThread(V4lCamera *camera_ptr,
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

    // May move the buffer reading here
  }
}

void V4lDriver::MainThread() {
  std::vector<V4lCamera *> enabled_cameras;
  for (const std::pair<std::string, V4lCamera *> & cam_name_cam: cameras_)
    if (cam_name_cam.second->Enabled())
      enabled_cameras.push_back(cam_name_cam.second);

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
    // struct sched_param is used to store the scheduling priority
    /*struct sched_param params;

    // We'll set the priority to the maximum.
    params.sched_priority = sched_get_priority_max(SCHED_FIFO);
    pthread_setschedparam(enabled_camera_threads[j]->native_handle(), SCHED_FIFO, &params);
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    for (int i = 6 / enabled_cameras.size() * j; i < 6 / enabled_cameras.size() * (j + 1); ++i) {
      CPU_SET(i, &cpuset);
    }
    int rc = pthread_setaffinity_np(enabled_camera_threads[j]->native_handle(),
                                    sizeof(cpu_set_t), &cpuset);*/
  }
  std::ofstream valod("diff.txt");

  while (!cancelled_) {

    // Wait until all cameras are ready
    bool all_are_ready;
    do {
      all_are_ready = true;
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      for (int j = 0; j < enabled_cameras.size(); ++j) {
        all_are_ready = all_are_ready && ready[j];
      }
    } while (!cancelled_ && !all_are_ready);
    if (cancelled_)
      break;

    for (int j = 0; j < enabled_cameras.size(); ++j) {
      capture_expected[j] = true;
    }

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

}
}
}