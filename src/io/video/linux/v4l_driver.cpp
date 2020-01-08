//
// Created by vahagn on 1/2/20.
//

#include "v4l_driver.h"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <sys/ioctl.h>
#include <sys/mman.h>
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
  InitDevNames(cameras);

  for (CameraDeviceInfo & info : cameras)
    if (!info.broken)
      devices_.push_back(info);

  camera_settings_.resize(devices_.size());
  for (int i = 0; i < devices_.size(); ++i) {
    camera_settings_[i].status = Enabled;
    camera_settings_[i].camera_name = devices_[i].device_path;
  }

}

void V4lDriver::FindAvailableCameras(std::vector<CameraDeviceInfo> & out_cameras) {
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

void V4lDriver::Open(std::vector<CameraDeviceInfo> & devices) {

  for (CameraDeviceInfo & info: devices) {
    info.fd = ::open(info.device_path.c_str(), O_RDWR);
    if (info.fd < 0) {
      info.broken = true;
      std::cerr << "Could not Open camera " + info.device_path << std::endl;
      std::cerr << (strerror(errno)) << std::endl;
    }
  }
}

void V4lDriver::InitAvailableFormats(std::vector<CameraDeviceInfo> & out_cameras) {
  for (CameraDeviceInfo & info: out_cameras) {
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

void V4lDriver::InitAvailableResolutions(std::vector<CameraDeviceInfo> & out_cameras) {
  v4l2_frmsizeenum argp;

  for (CameraDeviceInfo & info: out_cameras) {
    if (info.broken)
      continue;
    info.resolutions.resize(info.formats.size());
    for (const v4l2_fmtdesc & format: info.formats) {
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

void V4lDriver::InitDevNames(std::vector<CameraDeviceInfo> & out_cameras) {
  for (CameraDeviceInfo & info: out_cameras) {
    if (info.broken)
      continue;
    boost::filesystem::path b_path(info.device_path);
    std::ifstream istream((boost::filesystem::path("/sys/class/video4linux") / b_path.filename() / "name").string());
    info.manufacturer = std::string((std::istreambuf_iterator<char>(istream)),
                                    std::istreambuf_iterator<char>());
    boost::algorithm::trim(info.manufacturer);
  }
}

void V4lDriver::PrepareBuffers() {

}

void V4lDriver::SetFormats() {
  current_formats.resize(devices_.size());
  for (int j = 0; j < devices_.size(); ++j) {
    memset(&current_formats[j], 0, sizeof(v4l2_format));
    current_formats[j].type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    current_formats[j].fmt.pix.width =
        devices_[j].resolutions[camera_settings_[j].format_index][camera_settings_[j].resolution_index].discrete.width;
    current_formats[j].fmt.pix.height =
        devices_[j].resolutions[camera_settings_[j].format_index][camera_settings_[j].resolution_index].discrete.height;
    current_formats[j].fmt.pix.pixelformat = devices_[j].formats[camera_settings_[j].format_index].pixelformat;
    current_formats[j].fmt.pix.field = V4L2_FIELD_NONE;
    if (0 != xioctl(devices_[j].fd, VIDIOC_S_FMT, &current_formats[j])) {
      std::cerr << "Could not set format for camera " + devices_[j].device_path << std::endl;
      std::cerr << (strerror(errno)) << std::endl;
      devices_[j].broken = true;
    }
  }
}
bool V4lDriver::SetSettings(const std::vector<CameraSettings> & settings) {

  if (settings.size() != devices_.size())
    return false;
  for (int i = 0; i < devices_.size(); ++i) {
    if (settings[i].resolution_index >= devices_[i].resolutions.size())
      return false;
  }
  camera_settings_ = settings;
  return true;

}

void V4lDriver::PrepareCapture() {
  SetFormats();
  InitRequestBuffers();
  InitBuffers();
}

void V4lDriver::Register(const algorithm::Observer<Capture> *observer) {

}
void V4lDriver::InitRequestBuffers() {
  current_request_buffers.resize(devices_.size());

  for (int j = 0; j < devices_.size(); ++j) {
    memset(&current_request_buffers[j], 0, sizeof(v4l2_requestbuffers));
    current_request_buffers[j].count = camera_settings_[j].number_of_buffers;
    current_request_buffers[j].type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    current_request_buffers[j].memory = V4L2_MEMORY_MMAP;

    if (-1 == xioctl(devices_[j].fd, VIDIOC_REQBUFS, &current_request_buffers[j])) {
      std::cerr << "Could not initialize request buffer for camera " + devices_[j].device_path << std::endl;
      std::cerr << (strerror(errno)) << std::endl;
      devices_[j].broken = true;
    }
  }
}

void V4lDriver::InitBuffers() {
  current_buffers.resize(devices_.size());
  buffers.resize(devices_.size());
  for (int j = 0; j < devices_.size(); ++j) {
    memset(&current_buffers[j], 0, sizeof(v4l2_buffer));
    current_buffers[j].type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    current_buffers[j].memory = V4L2_MEMORY_MMAP;
    current_buffers[j].index = 0;

    if (0 != xioctl(devices_[j].fd, VIDIOC_QUERYBUF, &current_buffers[j])) {
      std::cerr << "Could not initialize buffer for camera " + devices_[j].device_path << std::endl;
      std::cerr << (strerror(errno)) << std::endl;
      devices_[j].broken = true;
      continue;
    }

    buffers[j] = (uint8_t *) mmap(NULL, current_buffers[j].length, PROT_READ | PROT_WRITE,
                               MAP_SHARED, devices_[j].fd, current_buffers[j].m.offset);

    if (-1 == xioctl(devices_[j].fd, VIDIOC_STREAMON, &(current_buffers[j].type))) {
      std::cerr << "Could not Retrieve buffer for camera " + devices_[j].device_path << std::endl;
      std::cerr << (strerror(errno)) << std::endl;
      devices_[j].broken = true;
      continue;
    }

  }

}

void V4lDriver::Start() {
  PrepareCapture();

}

}
}
}