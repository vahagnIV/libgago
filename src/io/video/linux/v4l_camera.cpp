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
#include <linux/videodev2.h>

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

#define CLIP(color) (uint8_t)(((color) > 0xFF) ? 0xff : (((color) < 0) ? 0 : (color)))
void V4lCamera::Yuyv2Rgb(const uint8_t *src,
                         uint8_t *dest,
                         int width, int height,
                         int stride) {
  int j;

  while (--height >= 0) {
    for (j = 0; j + 1 < width; j += 2) {
      int u = src[1];
      int v = src[3];

      int u1 = (((u - 128) << 7) + (u - 128)) >> 6;
      int rg = (((u - 128) << 1) + (u - 128) +
          ((v - 128) << 2) + ((v - 128) << 1)) >> 3;

      int v1 = (((v - 128) << 1) + (v - 128)) >> 1;

      *dest++ = CLIP(src[0] + v1);
      *dest++ = CLIP(src[0] - rg);
      *dest++ = CLIP(src[0] + u1);

      *dest++ = CLIP(src[2] + v1);
      *dest++ = CLIP(src[2] - rg);
      *dest++ = CLIP(src[2] + u1);
      src += 4;
    }
    src += stride - (width * 2);
  }
}

V4lCamera::V4lCamera(int fd,
                     const std::string & device_path,
                     const std::string & device_details,
                     const std::vector<v4l2_fmtdesc> & format_descriptions,
                     const std::vector<std::vector<v4l2_frmsizeenum>> & available_resolutions)
    : fd_(fd),
      format_descriptions_(format_descriptions),
      available_resolutions_(available_resolutions),
      device_details_(device_details),
      current_capture_index(0),
      device_path_(device_path) {

  for (v4l2_fmtdesc & fmt : format_descriptions_)
    formats_.push_back(std::string((char *) fmt.description));

  resolutions_.resize(formats_.size());
  for (int i = 0; i < formats_.size(); ++i) {
    resolutions_[i].resize(available_resolutions[i].size());
    for (int j = 0; j < available_resolutions[i].size(); ++j) {
      resolutions_[i][j].width = available_resolutions[i][j].discrete.width;
      resolutions_[i][j].height = available_resolutions[i][j].discrete.height;
    }
  }

  SetName(device_path);
  settings_.status = CameraStatus::Enabled;
  settings_.format_index = 0;
  settings_.resolution_index = 0;
  settings_.number_of_buffers = 3;

  memset(&request_buffers_, 0, sizeof(request_buffers_));

}

void V4lCamera::InitFormats(int fd, std::vector<v4l2_fmtdesc> & out_formats) {
  v4l2_fmtdesc format_description;
  format_description.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  format_description.index = 0;

  while (0 == xioctl(fd, VIDIOC_ENUM_FMT, &format_description)) {
    out_formats.push_back(format_description);
    format_description.index++;
  }
}

void V4lCamera::InitResolutions(int fd,
                                const std::vector<v4l2_fmtdesc> & formats,
                                std::vector<std::vector<v4l2_frmsizeenum>> & resolutions) {
  v4l2_frmsizeenum argp;

  resolutions.resize(formats.size());
  for (const v4l2_fmtdesc & format: formats) {
    argp.index = 0;
    argp.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    argp.pixel_format = format.pixelformat;
    while (0 == xioctl(fd, VIDIOC_ENUM_FRAMESIZES, &argp)) {
      resolutions[format.index].push_back(argp);
      argp.index++;
    }
  }

}

V4lCamera *V4lCamera::Create(const std::string & device_path) {
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

V4lCamera::~V4lCamera() {
  UnmapBuffers();
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
      available_resolutions_[settings_.format_index][settings_.resolution_index].discrete.width;
  format_.fmt.pix.height =
      available_resolutions_[settings_.format_index][settings_.resolution_index].discrete.height;
  format_.fmt.pix.pixelformat = format_descriptions_[settings_.format_index].pixelformat;
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
  request_buffers_.count = settings_.number_of_buffers;
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
  buffers_.resize(settings_.number_of_buffers);
  v4l2_buffer_.resize(settings_.number_of_buffers);
  for (int i = 0; i < settings_.number_of_buffers; ++i) {

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

const std::string & V4lCamera::GetUniqueId() const {
  return device_path_;
}

const std::string & V4lCamera::GetHardwareDetails() const {
  return device_details_;
}

bool V4lCamera::PrepareBuffers() {
  return SetFormat() && InitRequestBuffers() && InitBuffers();
}

bool V4lCamera::Grab() {
  bool result = 0 == xioctl(fd_, VIDIOC_QBUF, &v4l2_buffer_[current_capture_index]);
}

bool V4lCamera::Retieve(cv::Mat & out_image) {

  if (-1 == xioctl(fd_, VIDIOC_DQBUF, &v4l2_buffer_[current_capture_index]))
    return false;

  out_image.create(format_.fmt.pix.height, format_.fmt.pix.width, CV_8UC3);

  Yuyv2Rgb(buffers_[current_capture_index],
           out_image.data,
           format_.fmt.pix.width,
           format_.fmt.pix.height,
           format_.fmt.pix.bytesperline);

  current_capture_index = (current_capture_index + 1) % settings_.number_of_buffers;
  return true;

}
void V4lCamera::UnmapBuffers() {
  if (request_buffers_.count) {
    request_buffers_.count = 0;
    for (int j = 0; j < v4l2_buffer_.size(); ++j) {
      if (0 == xioctl(fd_, VIDIOC_STREAMOFF, &(v4l2_buffer_[j].type))
          && 0 == munmap(buffers_[j], v4l2_buffer_[j].length)) {
      } else {
        std::cerr << "Could not unmap buffer for camera " + device_path_ << std::endl;
        std::cerr << (strerror(errno)) << std::endl;
      }
    }
    buffers_.resize(0);
    v4l2_buffer_.resize(0);

    if (-1 == xioctl(fd_, VIDIOC_REQBUFS, &request_buffers_)) {
      std::cerr << "Could not initialize request buffer for camera " + device_path_ << std::endl;
      std::cerr << (strerror(errno)) << std::endl;
      return;
    }
  }

}

}
}
}