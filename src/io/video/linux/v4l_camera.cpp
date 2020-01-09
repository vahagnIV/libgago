//
// Created by vahagn on 1/9/20.
//

#include "v4l_camera.h"
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <libv4l2.h>
#include <boost/filesystem/path.hpp>
#include <boost/algorithm/string/trim.hpp>

namespace gago {
namespace io {
namespace video {

int xioctl(int fd, int request, void *arg) {
  int r;

  do
    r = ioctl(fd, request, arg);
  while (-1 == r && EINTR == errno);

  return r;
}

V4lCamera::V4lCamera(int fd,
                     const std::string &device_path,
                     const std::string &device_details,
                     const std::vector<v4l2_fmtdesc> &format_descriptions,
                     const std::vector<std::vector<v4l2_frmsizeenum>> &available_resolutions)
    : fd_(fd),
      format_descriptions_(format_descriptions),
      available_resolutions_(available_resolutions),
      device_details_(device_details),
      device_path_(device_path) {

  for (v4l2_fmtdesc &fmt : format_descriptions_)
    formats_.push_back(std::string((char *) fmt.description));

  resolutions_.resize(formats_.size());
  for (int i = 0; i < formats_.size(); ++i) {
    resolutions_[i].resize(available_resolutions[i].size());
    for (int j = 0; j < available_resolutions[i].size(); ++j) {
      resolutions_[i][j].width = available_resolutions[i][j].discrete.width;
      resolutions_[i][j].height = available_resolutions[i][j].discrete.height;
    }
  }

  device_settings_.camera_name = device_path;
  device_settings_.status = Enabled;
  device_settings_.format_index = 0;
  device_settings_.resolution_index = 0;

}

void V4lCamera::InitFormats(int fd, std::vector<v4l2_fmtdesc> &out_formats) {
  v4l2_fmtdesc format_description;
  format_description.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  format_description.index = 0;

  while (0 == xioctl(fd, VIDIOC_ENUM_FMT, &format_description)) {
    out_formats.push_back(format_description);
    format_description.index++;
  }
}

void V4lCamera::InitResolutions(int fd,
                                const std::vector<v4l2_fmtdesc> &formats,
                                std::vector<std::vector<v4l2_frmsizeenum>> &resolutions) {
  v4l2_frmsizeenum argp;

  resolutions.resize(formats.size());
  for (const v4l2_fmtdesc &format: formats) {
    argp.index = 0;
    argp.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    argp.pixel_format = format.pixelformat;
    while (0 == xioctl(fd, VIDIOC_ENUM_FRAMESIZES, &argp)) {
      resolutions[format.index].push_back(argp);
      argp.index++;
    }
  }

}

V4lCamera *V4lCamera::Create(const std::string &device_path) {
  int fd = ::open(device_path.c_str(), O_RDWR);
  if (fd < 0)
    return nullptr;

  std::vector<v4l2_fmtdesc> formats;
  InitFormats(fd, formats);

  if (formats.size() == 0)
    return nullptr;
  std::vector<std::vector<v4l2_frmsizeenum>> resolutions;
  InitResolutions(fd, formats, resolutions);
  if (resolutions.size() == 0)
    return nullptr;

  boost::filesystem::path b_path(device_path);
  std::ifstream istream((boost::filesystem::path("/sys/class/video4linux") / b_path.filename() / "name").string());
  std::string details((std::istreambuf_iterator<char>(istream)),
                      std::istreambuf_iterator<char>());
  boost::algorithm::trim(details);

  return new V4lCamera(fd, device_path, details, formats, resolutions);

}

/*const CameraDeviceInfo *V4lCamera::GetInfo() const {
  return &device_info_;
}

const CameraSettings *V4lCamera::GetSettings() const {
  return &device_settings_;
}*/
V4lCamera::~V4lCamera() {
  Close();

}

bool V4lCamera::Open() {
  if (fd_ > 0)
    Close();
  fd_ = ::open(device_path_.c_str(), O_RDWR);
  return fd_ > 0;
}

void V4lCamera::Close() {
  if (fd_ > 0) {
    ::close(fd_);
    fd_ = 0;
  }

}

bool V4lCamera::SetFormat() {
  memset(&format_, 0, sizeof(v4l2_format));
  format_.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  format_.fmt.pix.width =
      available_resolutions_[device_settings_.format_index][device_settings_.resolution_index].discrete.width;
  format_.fmt.pix.height =
      available_resolutions_[device_settings_.format_index][device_settings_.resolution_index].discrete.height;
  format_.fmt.pix.pixelformat = format_descriptions_[device_settings_.format_index].pixelformat;
  format_.fmt.pix.field = V4L2_FIELD_NONE;
  if (0 != xioctl(fd_, VIDIOC_S_FMT, &format_)) {
    std::cerr << "Could not set format for camera " + device_path_ << std::endl;
    std::cerr << (strerror(errno)) << std::endl;
    return false;
  }
  return true;
}

bool V4lCamera::InitRequestBuffers() {
  memset(&request_buffers_, 0, sizeof(v4l2_requestbuffers));
  request_buffers_.count = device_settings_.number_of_buffers;
  request_buffers_.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  request_buffers_.memory = V4L2_MEMORY_MMAP;

  if (-1 == xioctl(fd_, VIDIOC_REQBUFS, &request_buffers_)) {
    std::cerr << "Could not initialize request buffer for camera " + device_path_ << std::endl;
    std::cerr << (strerror(errno)) << std::endl;
    return false;
  }

  return true;
}

bool V4lCamera::InitBuffers() {
  buffers_.resize(device_settings_.number_of_buffers);
  v4l2_buffer_.resize(device_settings_.number_of_buffers);
  for (int i = 0; i < device_settings_.number_of_buffers; ++i) {

    memset(&v4l2_buffer_[i], 0, sizeof(v4l2_buffer));
    v4l2_buffer_[i].type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    v4l2_buffer_[i].memory = V4L2_MEMORY_MMAP;
    v4l2_buffer_[i].index = i;

    if (0 != xioctl(fd_, VIDIOC_QUERYBUF, &v4l2_buffer_[i])) {
      std::cerr << "Could not initialize buffer for camera " + device_path_ << std::endl;
      std::cerr << (strerror(errno)) << std::endl;
      return false;
    }

    buffers_[i] = (uint8_t *) mmap(NULL, v4l2_buffer_[i].length, PROT_READ | PROT_WRITE,
                                   MAP_SHARED, fd_, v4l2_buffer_[i].m.offset);

    // TODO: undo if anything goes wrong
    if (-1 == xioctl(fd_, VIDIOC_STREAMON, &(v4l2_buffer_[i].type))) {
      std::cerr << "Could not Retrieve buffer for camera " + device_path_ << std::endl;
      std::cerr << (strerror(errno)) << std::endl;
      return false;
    }
  }
}

}
}
}