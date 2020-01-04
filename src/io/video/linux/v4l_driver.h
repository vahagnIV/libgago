//
// Created by vahagn on 1/2/20.
//

#ifndef LIBGAGO_SRC_IO_VIDEO_V_4_L_DRIVER_H_
#define LIBGAGO_SRC_IO_VIDEO_V_4_L_DRIVER_H_

#include "io/video/icamera_driver.h"

namespace gago {
namespace io {
namespace video {

class V4lDriver : public ICameraDriver {
 public:
  V4lDriver();

  void Initialize();

  virtual ~V4lDriver() = default;
 protected:
  void Open(std::vector<CameraDeviceInfo> & devices);
  void FindAvailableCameras(std::vector<CameraDeviceInfo> & out_cameras);
  void InitAvailableFormats(std::vector<CameraDeviceInfo> & out_cameras);
  void InitAvailableResolutions(std::vector<CameraDeviceInfo> &out_cameras);
  void InitDevNames(std::vector<CameraDeviceInfo> &out_cameras);

};

}
}
}

#endif //LIBGAGO_SRC_IO_VIDEO_V_4_L_DRIVER_H_
