//
// Created by vahagn on 1/2/20.
//

#ifndef LIBGAGO_SRC_IO_VIDEO_CAMERA_H_
#define LIBGAGO_SRC_IO_VIDEO_CAMERA_H_

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include "camera_configuration.h"
#include "camera_meta.h"

namespace gago {
namespace io {
namespace video {

struct CameraSettings {
 public:
  CameraSettings(const CameraMeta *camera, const CameraConfiguration & config);
  CameraSettings(const CameraMeta *camera);

  const CameraMeta *camera;
  CameraConfiguration config;
};

}
}
}

#endif //LIBGAGO_SRC_IO_VIDEO_CAMERA_H_
