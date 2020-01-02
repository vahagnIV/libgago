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

void V4lDriver::Initialize() {
  std::vector<CameraDeviceInfo> cameras;
  FindAvailableCameras(cameras);
  cameras_ = cameras;
  camera_settings_.resize(cameras.size());

  // TODO: request camera capabilities
}

void V4lDriver::FindAvailableCameras(std::vector<CameraDeviceInfo> &out_cameras) {
  out_cameras.resize(0);

  if (!boost::filesystem::exists("/sys/class/video4linux"))
    return;
  for (boost::filesystem::directory_iterator it("/sys/class/video4linux");
       it != boost::filesystem::directory_iterator(); ++it) {
    if (boost::filesystem::is_directory(it->path())) {
      CameraDeviceInfo info;
      info.device_path = "/dev/" + it->path().filename().string();
      out_cameras.push_back(info);
    }
  }
}

}
}
}