//
// Created by vahagn on 1/2/20.
//

#ifndef LIBGAGO_SRC_IO_VIDEO_CAMERA_H_
#define LIBGAGO_SRC_IO_VIDEO_CAMERA_H_

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

namespace gago {
namespace io {
namespace video {

typedef cv::Size Size;

enum CameraStatus {
  Enabled = 1,
  Disabled = 2
};

std::string to_string(CameraStatus status);
bool try_parse(const std::string & status_str, CameraStatus & out_status);

struct CameraConfiguration {
  CameraStatus status;
  int resolution_index;
  std::string name;
  int format_index;
  int number_of_buffers = 1;
};

class CameraMeta;

struct CameraSettings {
  CameraSettings(const CameraMeta *camera, const CameraConfiguration & config) : camera(camera), config(config) {}
  const CameraMeta *camera;
  CameraConfiguration config;
};

class CameraMeta {
 public:
  virtual const std::string & GetName() const { return settings_.name; }
  virtual void SetName(const std::string & name) { settings_.name = name; }
  virtual const std::string & GetUniqueId() const = 0;
  virtual const std::string & GetHardwareDetails() const = 0;
  virtual const std::vector<std::vector<Size>> & GetResolutions() const { return resolutions_; }
  virtual const std::vector<std::string> & GetFormats() const { return formats_; }
  const CameraConfiguration & GetConfiguration() const { return settings_; };
 protected:

  std::vector<std::vector<Size>> resolutions_;
  std::vector<std::string> formats_;
  virtual ~CameraMeta() = default;
  CameraConfiguration settings_;
};

}
}
}

#endif //LIBGAGO_SRC_IO_VIDEO_CAMERA_H_
