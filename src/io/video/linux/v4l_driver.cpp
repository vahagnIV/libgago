//
// Created by vahagn on 1/2/20.
//

#include "v4l_driver.h"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "v4l_camera.h"

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

  for (CameraDeviceInfo &info : cameras)
    if (!info.broken)
      devices_.push_back(info);

  camera_settings_.resize(devices_.size());
  for (int i = 0; i < devices_.size(); ++i) {
    camera_settings_[i].status = Enabled;
    camera_settings_[i].camera_name = devices_[i].device_path;
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

void V4lDriver::InitDevNames(std::vector<CameraDeviceInfo> &out_cameras) {
  for (CameraDeviceInfo &info: out_cameras) {
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

bool V4lDriver::SetSettings(const std::vector<CameraSettings> &settings) {

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

}

void V4lDriver::Register(const algorithm::Observer<Capture> *observer) {

}

void V4lDriver::Start() {

  cameras_.resize(0);
  for (int j = 0; j < devices_.size(); ++j) {
    if (camera_settings_[j].status == Enabled) {
      V4lCamera *camera = new V4lCamera(&devices_[j], &camera_settings_[j]);
      if (camera->SetFormat() && camera->InitRequestBuffers() && camera->InitBuffers())
        cameras_.push_back(camera);
    }
  }

}

int xioctl(int fd, int request, void *arg) {
  int r;

  do
    r = ioctl(fd, request, arg);
  while (-1 == r && EINTR == errno);

  return r;
}

V4lDriver::V4lCamera::V4lCamera(CameraDeviceInfo *device_invo, CameraSettings *settings)
    : device_info_(device_invo), device_settings_(settings) {

}

bool V4lDriver::V4lCamera::SetFormat() {
  memset(&format_, 0, sizeof(v4l2_format));
  format_.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  format_.fmt.pix.width =
      device_info_->resolutions[device_settings_->format_index][device_settings_->resolution_index].discrete.width;
  format_.fmt.pix.height =
      device_info_->resolutions[device_settings_->format_index][device_settings_->resolution_index].discrete.height;
  format_.fmt.pix.pixelformat = device_info_->formats[device_settings_->format_index].pixelformat;
  format_.fmt.pix.field = V4L2_FIELD_NONE;
  if (0 != xioctl(device_info_->fd, VIDIOC_S_FMT, &format_)) {
    std::cerr << "Could not set format for camera " + device_info_->device_path << std::endl;
    std::cerr << (strerror(errno)) << std::endl;
    return false;
  }
  return true;
}

bool V4lDriver::V4lCamera::InitRequestBuffers() {
  memset(&request_buffers_, 0, sizeof(v4l2_requestbuffers));
  request_buffers_.count = device_settings_->number_of_buffers;
  request_buffers_.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  request_buffers_.memory = V4L2_MEMORY_MMAP;

  if (-1 == xioctl(device_info_->fd, VIDIOC_REQBUFS, &request_buffers_)) {
    std::cerr << "Could not initialize request buffer for camera " + device_info_->device_path << std::endl;
    std::cerr << (strerror(errno)) << std::endl;
    return false;
  }
  return true;
}

bool V4lDriver::V4lCamera::InitBuffers() {
  buffers_.resize(device_settings_->number_of_buffers);
  v4l2_buffer_.resize(device_settings_->number_of_buffers);
  for (int i = 0; i < device_settings_->number_of_buffers; ++i) {

    memset(&v4l2_buffer_[i], 0, sizeof(v4l2_buffer));
    v4l2_buffer_[i].type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    v4l2_buffer_[i].memory = V4L2_MEMORY_MMAP;
    v4l2_buffer_[i].index = i;

    if (0 != xioctl(device_info_->fd, VIDIOC_QUERYBUF, &v4l2_buffer_[i])) {
      std::cerr << "Could not initialize buffer for camera " + device_info_->device_path << std::endl;
      std::cerr << (strerror(errno)) << std::endl;
      return false;
    }

    buffers_[i] = (uint8_t *) mmap(NULL, v4l2_buffer_[i].length, PROT_READ | PROT_WRITE,
                                   MAP_SHARED, device_info_->fd, v4l2_buffer_[i].m.offset);

    // TODO: undo if anything goes wrong
    if (-1 == xioctl(device_info_->fd, VIDIOC_STREAMON, &(v4l2_buffer_[i].type))) {
      std::cerr << "Could not Retrieve buffer for camera " + device_info_->device_path << std::endl;
      std::cerr << (strerror(errno)) << std::endl;
      return false;
    }
  }


  return true;
}

}
}
}