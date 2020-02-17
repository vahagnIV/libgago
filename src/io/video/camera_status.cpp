//
// Created by vahagn on 2/17/20.
//

#include "camera_status.h"

#include <boost/algorithm/string.hpp>

namespace gago {
namespace io {
namespace video {

std::string to_string(CameraStatus status) {
  switch (status) {
    case CameraStatus::Disabled:return "Disabled";
    case CameraStatus::Enabled: return "Enabled";
    default: return "";
  }
}

bool try_parse(const std::string &status_str, CameraStatus &out_status) {
  if (boost::algorithm::iequals(status_str, "Enabled")) {
    out_status = CameraStatus::Enabled;
    return true;
  }

  if (boost::algorithm::iequals(status_str, "Disabled")) {
    out_status = CameraStatus::Disabled;
    return true;
  }

  return false;
}

}
}
}