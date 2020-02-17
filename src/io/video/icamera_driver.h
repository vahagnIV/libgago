//
// Created by vahagn on 1/2/20.
//

#ifndef LIBGAGO_SRC_IO_VIDEO_I_CAMERA_DRIVER_H_
#define LIBGAGO_SRC_IO_VIDEO_I_CAMERA_DRIVER_H_

#include "camera_settings.h"
#include "icamera_watcher.h"

namespace gago {
namespace io {
namespace video {

class ICameraDriver {
 public:
  ICameraDriver() = default;
  virtual void SetSettings(const std::vector<CameraSettings> & settings) = 0;
  virtual void GetSettings(std::vector<CameraSettings> & out_settings) const = 0;
  virtual std::vector<const CameraMeta *> GetCameras() const = 0;
  virtual void RegisterWatcher(CameraWatcher *) = 0;
  virtual void UnRegister(CameraWatcher *) = 0;
  virtual void Start() = 0;
  virtual void Stop() = 0;
  virtual void Join() = 0;
  virtual ~ICameraDriver() = default;
};

}
}
}
#endif //LIBGAGO_SRC_IO_VIDEO_I_CAMERA_DRIVER_H_
