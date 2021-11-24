//
// Created by vahagn on 2/17/20.
//

#ifndef LIBGAGO_CAPTURE_H
#define LIBGAGO_CAPTURE_H
#include "camera_meta.h"

namespace gago {
namespace io {
namespace video {

struct Capture {
  Capture(const CameraMeta *cam) : camera(cam) {}
  const CameraMeta *camera;
  long long capture_date;
  std::vector<uint8_t> data;
  uint32_t width;
  uint32_t height;
  uint8_t channels;
};

}
}
}

#endif //LIBGAGO_CAPTURE_H
