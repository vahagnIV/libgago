//
// Created by Vahagn Yeghikyan on 2020-03-01.
//

#include "opencv_camera.h"
namespace gago {
namespace io {
namespace video {

OpenCvCamera *OpenCvCamera::Create(int dev_id) {
  OpenCvCamera * camera =  new OpenCvCamera(dev_id);
  if (camera->device_.isOpened())
    return camera;
  return nullptr;
}

OpenCvCamera::OpenCvCamera(int dev_id) : device_(dev_id) {
  device_.open(dev_id);

  int width = device_.get(cv::CAP_PROP_FRAME_WIDTH);
  int height = device_.get(cv::CAP_PROP_FRAME_HEIGHT);
  this->resolutions_.push_back({Size(width, height)});
  uid_ = std::to_string(dev_id);

}

const std::string &OpenCvCamera::GetUniqueId() const {
  return uid_;
}

const std::string &OpenCvCamera::GetHardwareDetails() const {
  return details_;
}

void OpenCvCamera::SetConfiguration(const CameraConfiguration &config) {

}

bool OpenCvCamera::Grab() {
  return device_.grab();
}
bool OpenCvCamera::Enabled() {
  return true;
}

bool OpenCvCamera::Retieve(cv::Mat &out_data) {
  return device_.retrieve(out_data);
}

OpenCvCamera::~OpenCvCamera() {
  device_.release();
}
}
}
}
