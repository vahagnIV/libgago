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
  Capture(const CameraMeta *cam) : camera(cam) {}
  const CameraMeta *camera;
  long long capture_date;
  cv::Mat data;
};

// TODO: remove observer
class CameraWatcher : public algorithm::Observer<std::vector<Capture>> {
 public:
  virtual void SetCameras(const std::vector<const CameraMeta *> &) = 0;
};

class ICameraDriver : public algorithm::Observable<std::vector<Capture>> {
 public:
  ICameraDriver() = default;
  virtual void SetSettings(const std::vector<CameraSettings> & settings) = 0;
  virtual void GetSettings(std::vector<CameraSettings> & out_settings) const = 0;
  virtual std::vector<const CameraMeta *> GetCameras() const = 0;
  virtual void RegisterWatcher(CameraWatcher *) = 0;
  virtual void UnRegister(CameraWatcher *) = 0;
  virtual void Start() = 0;
  virtual ~ICameraDriver() = default;
 protected:

};
}
}
}
#endif //LIBGAGO_SRC_IO_VIDEO_I_CAMERA_DRIVER_H_
