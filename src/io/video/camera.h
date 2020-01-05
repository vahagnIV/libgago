//
// Created by vahagn on 1/2/20.
//

#ifndef LIBGAGO_SRC_IO_VIDEO_CAMERA_H_
#define LIBGAGO_SRC_IO_VIDEO_CAMERA_H_

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <linux/videodev2.h>

namespace gago {
namespace io {
namespace video {

typedef cv::Size Size;

enum CameraStatus {
  Enabled = 1,
  Disabled = 2
};

std::string to_string(CameraStatus status);
bool try_parse(const std::string & status_str, CameraStatus & out_status);

struct CameraDeviceInfo {
  std::string manufacturer;
  std::string device_path;
  bool broken = false;

  // TODO: think what
  std::vector<v4l2_fmtdesc> formats;
  std::vector<std::vector<v4l2_frmsizeenum>> resolutions;
  int fd = 0;
};

struct CameraSettings {
  CameraStatus status;
  std::string camera_name;
  int format_index;
  int resolution_index;
};

}
}
}

#endif //LIBGAGO_SRC_IO_VIDEO_CAMERA_H_
