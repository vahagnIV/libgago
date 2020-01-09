//
// Created by vahagn on 1/2/20.
//

#ifndef LIBGAGO_SRC_IO_VIDEO_V_4_L_DRIVER_H_
#define LIBGAGO_SRC_IO_VIDEO_V_4_L_DRIVER_H_

#include "io/video/icamera_driver.h"

namespace gago {
namespace io {
namespace video {

class V4lDriver;
/*


class V4lDriver : public ICameraDriver {
 public:
  // ICamera
  bool SetSettings(const std::vector<CameraSettings> &settings) override;

  //Observable
  void Register(const algorithm::Observer<Capture> *observer) override;
  void Start();

  V4lDriver();
  void Initialize();
  virtual ~V4lDriver() = default;
 protected:
  void PrepareBuffers();

 protected:

  class V4lCamera {
    friend class V4lDriver;

    V4lCamera(CameraDeviceInfo *device_invo, CameraSettings *settings, int fd = 0);
    bool Open();
    void Close();
    bool IsOpen();
    bool SetFormat();
    bool InitRequestBuffers();
    bool InitBuffers();

    CameraDeviceInfo *device_info_;
    CameraSettings *device_settings_;
    v4l2_format format_;
    v4l2_requestbuffers request_buffers_;
    std::vector<v4l2_buffer> v4l2_buffer_;
    std::vector<uint8_t *> buffers_;
    int fd_;

  };

  void PrepareCapture();
  void Open(std::vector<CameraDeviceInfo> &devices);
  void FindAvailableCameras(std::vector<CameraDeviceInfo> &out_cameras);
  void InitAvailableFormats(std::vector<CameraDeviceInfo> &out_cameras);
  void InitAvailableResolutions(std::vector<CameraDeviceInfo> &out_cameras);
  void InitDevNames(std::vector<CameraDeviceInfo> &out_cameras);
  std::vector<V4lCamera *> cameras_;

};
*/
}
}
}

#endif //LIBGAGO_SRC_IO_VIDEO_V_4_L_DRIVER_H_
