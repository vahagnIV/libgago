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
  CameraMeta *camera;
  cv::Mat data;
};

class ICameraDriver : public algorithm::Observable<Capture> {

 public:
  ICameraDriver() = default;
  virtual std::vector<CameraMeta *> GetMeta()  = 0;
  virtual ~ICameraDriver() = default;
 protected:


};
}
}
}
#endif //LIBGAGO_SRC_IO_VIDEO_I_CAMERA_DRIVER_H_
