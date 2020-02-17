//
// Created by vahagn on 1/2/20.
//

#ifndef LIBGAGO_SRC_IO_VIDEO_V_4_L_DRIVER_H_
#define LIBGAGO_SRC_IO_VIDEO_V_4_L_DRIVER_H_
#include <atomic>
#include <thread>
#include <condition_variable>
#include <mutex>

#include "io/video/linux/v4l_camera.h"
#include "io/video/icamera_driver.h"

namespace gago {
namespace io {
namespace video {

class V4lDriver : public ICameraDriver {
 public:
  V4lDriver();
  void Initialize();

  //ICameraDriver
  void SetSettings(const std::vector<CameraSettings> & settings) override;
  void GetSettings(std::vector<CameraSettings> & out_settings) const override;
  void Start() override;
  void RegisterWatcher(CameraWatcher *observer) override;
  void UnRegister(CameraWatcher *observer) override;
  void Stop() override;
  void Join() override;
  std::vector<const CameraMeta *> GetCameras() const override;

  virtual ~V4lDriver();
 private:
  void CaptureThread(V4lCamera *camera_ptr,
                     std::atomic_bool & capture_requested,
                     std::atomic_bool & ready,
                     long long & time);
  void Notify(const std::shared_ptr<std::vector<Capture>> & captures);
  void MainThread();

  std::unordered_map<std::string, V4lCamera *> cameras_;
  std::vector<CameraWatcher *> watchers_;
  std::atomic_bool cancelled_;
  std::thread *thread_ = nullptr;
  std::mutex mutex_;
  std::condition_variable condition_variable_;

};

}
}
}

#endif //LIBGAGO_SRC_IO_VIDEO_V_4_L_DRIVER_H_
