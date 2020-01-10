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

class V4lDriver;

class V4lCamera : public CameraMeta {
 public:
  virtual ~V4lCamera();
  static V4lCamera *Create(const std::string & device_path);
  const std::string & GetUniqueId() const override;
  const std::string & GetHardwareDetails() const override;
  friend class V4lDriver;

  bool Enabled() const { return settings_.status == CameraStatus::Enabled; }
  bool Grab();
  bool Retieve(cv::Mat & out_image);
  bool PrepareBuffers();
 private:
  bool Open();
  void Close();
  bool SetFormat();
  bool InitRequestBuffers();
  bool InitBuffers();

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

  void Yuyv2Rgb(const uint8_t *src,
                           uint8_t *dest,
                           int width, int height,
                           int stride);

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
