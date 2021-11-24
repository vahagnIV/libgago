//
// Created by vahagn on 1/9/20.
//

#ifndef LIBGAGO_V_4_L_CAMERA_H
#define LIBGAGO_V_4_L_CAMERA_H

#include "io/video/camera_settings.h"

#include <linux/videodev2.h>

namespace gago {
namespace io {
namespace video {

class V4lDriver;

class V4lCamera : public CameraMeta {
  friend class V4lDriver;
 public:
  virtual ~V4lCamera();
  static V4lCamera *Create(const std::string & device_path);

  //CameraMeta
  const std::string & GetUniqueId() const override;
  const std::string & GetHardwareDetails() const override;

  bool Enabled() const { return settings_.status == CameraStatus::Enabled; }
  void SetConfiguration(const CameraConfiguration & config);
 private:
  bool Grab();
  bool Retieve(std::vector<uint8_t> & out_image);
  bool PrepareBuffers();
  void Close();
  bool SetFormat();
  bool InitRequestBuffers();
  bool InitBuffers();
  void UnmapBuffers();

  V4lCamera(int fd,
            const std::string & device_path,
            const std::string & device_details,
            const std::vector<v4l2_fmtdesc> & format_descriptions,
            const std::vector<std::vector<v4l2_frmsizeenum>> & available_resolutions);

 private:
  static void InitFormats(int fd, std::vector<v4l2_fmtdesc> & formats);
  static void InitResolutions(int fd,
                              const std::vector<v4l2_fmtdesc> & formats,
                              std::vector<std::vector<v4l2_frmsizeenum>> & resolutions);



  // Device info
  const std::string device_path_;
  const std::string device_details_;

  // v4l
  std::vector<v4l2_fmtdesc> format_descriptions_;
  std::vector<std::vector<v4l2_frmsizeenum>> available_resolutions_;
  std::vector<uint8_t *> buffers_;
  v4l2_format format_;
  v4l2_requestbuffers request_buffers_;
  std::vector<v4l2_buffer> v4l2_buffer_;
  int fd_;
  int current_capture_index;

};

}
}
}
#endif //LIBGAGO_V_4_L_CAMERA_H
