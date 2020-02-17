//
// Created by vahagn on 2/17/20.
//

#ifndef LIBGAGO_CAMERA_STATUS_H
#define LIBGAGO_CAMERA_STATUS_H

#include <string>

namespace gago {
namespace io {
namespace video {

enum CameraStatus {
  Enabled = 1,
  Disabled = 2
};

std::string to_string(CameraStatus status);
bool try_parse(const std::string & status_str, CameraStatus & out_status);

}
}}

#endif //LIBGAGO_CAMERA_STATUS_H
