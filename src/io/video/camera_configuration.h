//
// Created by vahagn on 2/17/20.
//

#ifndef LIBGAGO_CAMERA_CONFIGURATION_H
#define LIBGAGO_CAMERA_CONFIGURATION_H

#include "camera_status.h"

namespace gago {
namespace io {
namespace video {

struct CameraConfiguration {
  bool operator==(const CameraConfiguration & other) const {
    return format_index == other.format_index && resolution_index == other.resolution_index
        && status == other.status && number_of_buffers == other.number_of_buffers && vertical_flip == other.vertical_flip;
  }

  bool operator!=(const CameraConfiguration & other) const {
    return !(this->operator==(other));
  }
  CameraStatus status = CameraStatus::Enabled;
  bool vertical_flip = false;
  int resolution_index = 0;
  std::string name;
  int format_index = 0;
  int number_of_buffers = 1;
};

}
}
}

#endif //LIBGAGO_CAMERA_CONFIGURATION_H
