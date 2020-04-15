//
// Created by Vahagn Yeghikyan on 2020-03-01.
//
#include <io/video/cross_platform/opencv_driver.h>
#include <io/video/linux/v4l_driver.h>
#include "io/video/cross_platform/opencv_camera.h"
#include "io/video/icamera_watcher.h"
namespace gago {
namespace io {
namespace video {

class TestWatcher : public CameraWatcher {
 public:
  void SetCameras(const std::vector<const CameraMeta *> & out_cameras) override {
    for (const CameraMeta *camer : out_cameras) {
      cv::namedWindow(camer->GetUniqueId(), cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
    }

  }

  void Notify(const std::shared_ptr<std::vector<Capture>> & data) override {
    for (Capture cap: *data) {
      if (cap.data.empty())
        continue;
      cv::imshow(cap.camera->GetUniqueId(), cap.data);
    }
    cv::waitKey(1);
  }
};

}
}
}

int main(int argc, char *argv[]) {
  gago::io::video::V4lDriver driver;
  driver.Initialize();
  auto cameras = driver.GetCameras();
  gago::io::video::CameraConfiguration configuration;
  configuration.format_index = 1;

  driver.SetSettings(std::vector<gago::io::video::CameraSettings>{
      gago::io::video::CameraSettings(cameras[0], configuration)});
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  gago::io::video::TestWatcher watcher;
  driver.RegisterWatcher(&watcher);
  driver.MainThread();
  driver.Join();
}