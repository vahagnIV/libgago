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
  // ICamera
  bool SetSettings(const std::vector<CameraSettings> & settings) override;

  //Observable
  void Register(const algorithm::Observer<Capture> *observer) override ;
  void Start();

  V4lDriver();
  void Initialize();
  virtual ~V4lDriver() = default;
 protected:
  void PrepareBuffers();
  void SetFormats();

 protected:

  void PrepareCapture() ;
  void Open(std::vector<CameraDeviceInfo> & devices);
  void FindAvailableCameras(std::vector<CameraDeviceInfo> & out_cameras);
  void InitAvailableFormats(std::vector<CameraDeviceInfo> & out_cameras);
  void InitAvailableResolutions(std::vector<CameraDeviceInfo> & out_cameras);
  void InitDevNames(std::vector<CameraDeviceInfo> & out_cameras);
  void InitRequestBuffers();
  void InitBuffers();
  std::vector<v4l2_format> current_formats;
  std::vector<v4l2_requestbuffers> current_request_buffers;
  std::vector<v4l2_buffer> current_buffers;
  std::vector<uint8_t *> buffers;

};

}
}
}

#endif //LIBGAGO_SRC_IO_VIDEO_V_4_L_DRIVER_H_
