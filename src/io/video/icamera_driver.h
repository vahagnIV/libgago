//
// Created by vahagn on 1/2/20.
//

#ifndef LIBGAGO_SRC_IO_VIDEO_I_CAMERA_DRIVER_H_
#define LIBGAGO_SRC_IO_VIDEO_I_CAMERA_DRIVER_H_

#include "camera.h"
#include "algorithm/observable.hpp"

namespace gago {
namespace io {
namespace video {

struct Capture {
  CameraDeviceInfo *camera;
  CameraSettings *camera_settings;
  cv::Mat data;
};


class ICameraDriver: public algorithm::Observable<Capture> {

 public:
  ICameraDriver() = default;
  const std::vector<CameraDeviceInfo> &GetDevices() const {
    return cameras_;
  }

  const std::vector<CameraSettings> &GetSettings() const {
    return camera_settings_;
  }

  bool SetSettings(const std::vector<CameraSettings> &settings) {
    if (settings.size() != cameras_.size())
      return false;
    for (int i = 0; i < cameras_.size(); ++i) {
      if (settings[i].resolution_index >= cameras_[i].supported_resolutions.size())
        return false;
    }
    return true;
  }
  virtual ~ICameraDriver()= default;
 protected:
  std::vector<CameraDeviceInfo> cameras_;
  std::vector<CameraSettings> camera_settings_;

};
}
}
}
#endif //LIBGAGO_SRC_IO_VIDEO_I_CAMERA_DRIVER_H_
