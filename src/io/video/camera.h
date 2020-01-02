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

struct CameraDeviceInfo {
  std::string manufacturer;
  std::string device_path;
  std::vector<Size> supported_resolutions;
  bool broken;
};

struct CameraSettings {
  CameraStatus status;
  std::string camera_name;
  int resolution_index;
};

}
}
}

#endif //LIBGAGO_SRC_IO_VIDEO_CAMERA_H_
