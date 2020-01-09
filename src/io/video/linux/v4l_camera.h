//
// Created by vahagn on 1/9/20.
//

#ifndef LIBGAGO_V_4_L_CAMERA_H
#define LIBGAGO_V_4_L_CAMERA_H

#include "io/video/camera.h"

#include <linux/videodev2.h>

namespace gago {
namespace io {
namespace video {


struct CameraSettings {
  CameraStatus status;
  std::string camera_name;
  int format_index;
  int resolution_index;
  int number_of_buffers = 1;
};

class V4lCamera : public CameraMeta {
 public:
  /*const CameraDeviceInfo *GetInfo() const override;
  const CameraSettings *GetSettings() const override;*/
  virtual ~V4lCamera();
  static V4lCamera *Create(const std::string &device_path);
  bool Enabled() { return enabled_; }

 private:
  bool Open();
  void Close();
  bool SetFormat();
  bool InitRequestBuffers();
  bool InitBuffers();

  V4lCamera(int fd,
            const std::string &device_path,
            const std::string &device_details,
            const std::vector<v4l2_fmtdesc> &format_descriptions,
            const std::vector<std::vector<v4l2_frmsizeenum>> &available_resolutions);
  static void InitFormats(int fd, std::vector<v4l2_fmtdesc> &formats);
  static void InitResolutions(int fd,
                              const std::vector<v4l2_fmtdesc> &formats,
                              std::vector<std::vector<v4l2_frmsizeenum>> &resolutions);

  const std::string device_path_;
  const std::string device_details_;

  CameraSettings device_settings_;
  std::vector<v4l2_fmtdesc> format_descriptions_;
  std::vector<std::vector<v4l2_frmsizeenum>> available_resolutions_;
  v4l2_format format_;
  v4l2_requestbuffers request_buffers_;
  std::vector<v4l2_buffer> v4l2_buffer_;
  std::vector<uint8_t *> buffers_;
  int fd_;
  int current_capture_index;
  bool enabled_;

};

}
}
}
#endif //LIBGAGO_V_4_L_CAMERA_H
