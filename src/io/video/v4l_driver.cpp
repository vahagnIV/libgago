//
// Created by vahagn on 1/2/20.
//

#include "v4l_driver.h"
#include <boost/filesystem.hpp>
#include <sys/ioctl.h>
#include <fcntl.h>

int xioctl(int fd, int request, void *arg) {
  int r;

  do
    r = ioctl(fd, request, arg);
  while (-1 == r && EINTR == errno);

  return r;
}

namespace gago {
namespace io {
namespace video {
V4lDriver::V4lDriver() {

}

void V4lDriver::Initialize() {
  std::vector<CameraDeviceInfo> cameras;
  std::vector<int> fds;
  FindAvailableCameras(cameras);
  Open(cameras);
  InitAvailableFormats(cameras);
  InitAvailableResolutions(cameras);

  for (CameraDeviceInfo &info : cameras)
    if (!info.broken)
      cameras_.push_back(info);

  camera_settings_.resize(cameras_.size());
  for (int i = 0; i < cameras_.size(); ++i) {
    camera_settings_[i].status = Enabled;
    camera_settings_[i].camera_name = cameras_[i].device_path;
    camera_settings_[i].format_index = 1;
  }

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

void V4lDriver::Open(std::vector<CameraDeviceInfo> &devices) {

  for (CameraDeviceInfo &info: devices) {
    info.fd = ::open(info.device_path.c_str(), O_RDWR);
    if (info.fd < 0) {
      info.broken = true;
      std::cerr << "Could not Open camera " + info.device_path << std::endl;
      std::cerr << (strerror(errno)) << std::endl;
    }
  }
}

void V4lDriver::InitAvailableFormats(std::vector<CameraDeviceInfo> &out_cameras) {
  for (CameraDeviceInfo &info: out_cameras) {
    if (info.broken)
      continue;
    v4l2_fmtdesc format_description;
    format_description.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    format_description.index = 0;
    while (0 == xioctl(info.fd, VIDIOC_ENUM_FMT, &format_description)) {
      info.formats.push_back(format_description);
      format_description.index++;
    }
    if (info.formats.empty())
      info.broken = true;
  }
}

void V4lDriver::InitAvailableResolutions(std::vector<CameraDeviceInfo> &out_cameras) {
  v4l2_frmsizeenum argp;

  for (CameraDeviceInfo &info: out_cameras) {
    if (info.broken)
      continue;
    info.resolutions.resize(info.formats.size());
    for (const v4l2_fmtdesc &format: info.formats) {
      argp.index = 0;
      argp.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      argp.pixel_format = format.pixelformat;
      while (0 == xioctl(info.fd, VIDIOC_ENUM_FRAMESIZES, &argp)) {
        info.resolutions[format.index].push_back(argp);
        argp.index++;
      }
      if (info.resolutions.size() == 0)
        info.broken = true;
    }
  }
}

}
}
}