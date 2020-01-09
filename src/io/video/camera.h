//
// Created by vahagn on 1/2/20.
//

#ifndef LIBGAGO_SRC_IO_VIDEO_CAMERA_H_
#define LIBGAGO_SRC_IO_VIDEO_CAMERA_H_

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

namespace gago {
namespace io {
namespace video {

typedef cv::Size Size;

enum CameraStatus {
  Enabled = 1,
  Disabled = 2
};

std::string to_string(CameraStatus status);
bool try_parse(const std::string &status_str, CameraStatus &out_status);

class CameraMeta {
 public:
  virtual const std::vector<std::vector<Size>> &GetResolutions() const { return resolutions_; }
  virtual const std::vector<std::string> &GetFormats() const { return formats_; }
  const std::string &GetName() const { return name_; }
  void SetName(const std::string &name) { name_ = name; }
 private:
  std::string name_;
  std::vector<std::vector<Size>> resolutions_;
  std::vector<std::string> formats_;
  virtual ~CameraMeta() = default;
};

}
}
}

#endif //LIBGAGO_SRC_IO_VIDEO_CAMERA_H_
