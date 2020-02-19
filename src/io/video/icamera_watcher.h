//
// Created by vahagn on 2/17/20.
//

#ifndef LIBGAGO_CAMERA_WATCHER_H
#define LIBGAGO_CAMERA_WATCHER_H

#include "camera_meta.h"
#include "capture.h"

namespace gago {
namespace io {
namespace video {

class CameraWatcher {
 public:
  /*!
   * This function is called by the camera driver
   * when the list of cameras was changed
   * @param out_cameras The list which will be list with vailid CameraMeta pointers
   */
  virtual void SetCameras(const std::vector<const CameraMeta *> & out_cameras) = 0;

  /*!
   * Watcher will be notified about new frames through this function
   * @param data shared pointer to the list of Capture objects
   */
  virtual void Notify(const std::shared_ptr<std::vector<Capture>> & data) = 0;

  virtual ~CameraWatcher() = default;
};

}
}
}

#endif //LIBGAGO_CAMERA_WATCHER_H
