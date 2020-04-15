//
// Created by Vahagn Yeghikyan on 2020-03-01.
//

#ifndef LIBGAGO_OPENCVCAMERA_H
#define LIBGAGO_OPENCVCAMERA_H

#include "io/video/camera_settings.h"
#include <opencv2/opencv.hpp>



namespace gago {
namespace io {
namespace video {

class OpenCvDriver;

class OpenCvCamera: public CameraMeta {
  friend class OpenCvDriver;
 public:
  const std::string &GetUniqueId() const override;
  const std::string &GetHardwareDetails() const override;
  static OpenCvCamera *Create(int dev_id);
  virtual ~OpenCvCamera();
 private:
  void SetConfiguration(const CameraConfiguration & config);
  OpenCvCamera(int dev_id);
  bool Grab();
  bool Retieve(cv::Mat & out_data);
  bool Enabled();
  cv::VideoCapture device_;
  std::string uid_;
  std::string details_ = "Mac OS camera";
};

}
}
}

#endif //LIBGAGO_OPENCVCAMERA_H
