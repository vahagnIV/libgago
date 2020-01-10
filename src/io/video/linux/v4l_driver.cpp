//
// Created by vahagn on 1/2/20.
//

#include "v4l_driver.h"
#include <boost/filesystem.hpp>

namespace gago {
namespace io {
namespace video {

V4lDriver::V4lDriver() {

}

void V4lDriver::Register(const algorithm::Observer<Capture> *observer) {

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
  for(const CameraSettings & setting: settings)
  {
    if(cameras_.find(setting.camera->GetUniqueId())!= cameras_.end())
      cameras_[setting.camera->GetUniqueId()]->SetName(setting.config.name);
  }
}

void V4lDriver::GetSettings(std::vector<CameraSettings> & out_settings) const {
  for(const std::pair<std::string, V4lCamera * > & camera: cameras_)
  {
    out_settings.push_back(CameraSettings(camera.second, camera.second->GetConfiguration()));
  }
}

void V4lDriver::Start() {
  for(const std::pair<std::string, V4lCamera * > & camera: cameras_)
  {
    if(camera.second->Enabled())
      camera.second->PrepareBuffers();
  }
}

void V4lDriver::CaptureThread(V4lCamera *camera_ptr) {
  while (!cancelled_)
  {
    std::unique_lock<std::mutex> lk(mutex_);
    condition_variable_.wait(lk, [&]{ return capture_expected_;});
    camera_ptr->Grab();
  }
}

void V4lDriver::MainThread(std::vector<V4lCamera * > cameras) {
  while (!cancelled_)
  {
    capture_expected_= true;
    condition_variable_.notify_all();
    // Wait for captrue threads to finish

    capture_expected_ = false;
    std::shared_ptr<std::vector<Capture>> captures = std::make_shared<std::vector<Capture>>();
    for (int i = 0; i < cameras.size(); ++i) {
      Capture capture(cameras[i]);
      cameras[i]->Retieve(capture.data);
      captures->push_back(capture);
    }
    
  }
}

}
}
}