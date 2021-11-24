//
// Created by vahagn on 2/17/20.
//

#ifndef LIBGAGO_CAMERA_META_H
#define LIBGAGO_CAMERA_META_H

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

#include "camera_configuration.h"

namespace gago {
namespace io {
namespace video {

struct Size{
  int width;
  int height;
};

class CameraMeta {
 public:
  /*!
   * Returns a string which uniquely describes the device for further reference
   * @return string
   */
  virtual const std::string & GetUniqueId() const = 0;

  /*!
   * Provides some additional device info in human readable format
   * @return string
   */
  virtual const std::string & GetHardwareDetails() const = 0;

  /*!
   * Return a custom set name for the camera. E.g. left, right...
   * @return string
   */
  virtual const std::string & GetName() const { return settings_.name; }

  /*!
   * Returns a list of available resolutions supported by the device
   * @return vector of resolutions
   */
  virtual const std::vector<std::vector<Size>> & GetResolutions() const { return resolutions_; }

  /*!
   * Returns a alist of supported formats. E.g. YUYV 4:2:2, Motion-JPEG etc.
   * @return
   */
  virtual const std::vector<std::string> & GetFormats() const { return formats_; }

  /*!
   * Returns the current configuration
   * @return CameraConfiguration structure
   */
  const CameraConfiguration & GetConfiguration() const { return settings_; };

  /*!
   * Virtual destructor
   */
  virtual ~CameraMeta() = default;
 protected:
  std::vector<std::vector<Size>> resolutions_;
  std::vector<std::string> formats_;
  CameraConfiguration settings_;
};

}
}
}

#endif //LIBGAGO_CAMERA_META_H
