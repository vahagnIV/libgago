//
// Created by vahagn on 2/17/20.
//

#include "camera_settings.h"

namespace gago {
namespace io {
namespace video {

CameraSettings::CameraSettings(const CameraMeta *camera, const CameraConfiguration & config)
    : camera(camera), config(config) {}

CameraSettings::CameraSettings(const CameraMeta *camera) : camera(camera) {}

}
}
}