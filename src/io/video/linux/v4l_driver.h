//
// Created by vahagn on 1/2/20.
//

#ifndef LIBGAGO_SRC_IO_VIDEO_V_4_L_DRIVER_H_
#define LIBGAGO_SRC_IO_VIDEO_V_4_L_DRIVER_H_

#include "io/video/linux/v4l_camera.h"
#include "io/video/icamera_driver.h"
#include <atomic>
#include <thread>
#include <condition_variable>
#include <mutex>

namespace gago {
namespace io {
namespace video {

class V4lDriver : public ICameraDriver {
 public:
  V4lDriver();

  // Observable
  void Register(const algorithm::Observer<Capture> *observer) override;

  void Initialize();
  void SetSettings(const std::vector<CameraSettings> & settings) override;
  void GetSettings(std::vector<CameraSettings> & out_settings) const override;
  virtual ~V4lDriver(){}
  void Start() override;

 private:
  void CaptureThread(V4lCamera * camera_ptr);
  void MainThread(std::vector<V4lCamera * > cameras);

  std::unordered_map<std::string, V4lCamera *> cameras_;
  std::atomic_bool cancelled_;
  bool capture_expected_;
  std::vector<std::thread *> threads_;
  std::mutex mutex_;
  std::condition_variable condition_variable_;

};

}
}
}

#endif //LIBGAGO_SRC_IO_VIDEO_V_4_L_DRIVER_H_
