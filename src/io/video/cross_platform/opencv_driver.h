//
// Created by Vahagn Yeghikyan on 2020-03-01.
//

#ifndef LIBGAGO_OPENCVDRIVER_H
#define LIBGAGO_OPENCVDRIVER_H
#include <thread>
#include <io/video/icamera_driver.h>
#include "opencv_camera.h"

namespace gago {
namespace io {
namespace video {
class OpenCvDriver : public ICameraDriver{
 public:
  void Initialize();
  void SetSettings(const std::vector<CameraSettings> &settings) override;
  void GetSettings(std::vector<CameraSettings> &out_settings) const override;
  std::vector<const CameraMeta *> GetCameras() const override;
  void RegisterWatcher(CameraWatcher *watcher) override;
  void UnRegister(CameraWatcher *watcher) override;
  void Start() override;
  void Stop() override;
  void Join() override;
  void MainThread();
 private:
  void CaptureThread(OpenCvCamera *camera_ptr,
                     std::atomic_bool & capture_requested,
                     std::atomic_bool & ready,
                     long long & time);
  void Notify(const std::shared_ptr<std::vector<Capture>> & captures);

  std::unordered_map<std::string, OpenCvCamera *> cameras_;
  std::vector<CameraWatcher *> watchers_;
  std::atomic_bool cancelled_;
  std::thread *thread_ = nullptr;
  std::mutex mutex_;
  std::condition_variable condition_variable_;

};
}}}

#endif //LIBGAGO_OPENCVDRIVER_H
