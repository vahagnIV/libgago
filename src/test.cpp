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
    for (const CameraMeta * camer: out_cameras) {
      cv::namedWindow(camer->GetUniqueId(), cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
    }

  }

  void Notify(const std::shared_ptr<std::vector<Capture>> & data) override {
    for (Capture cap: *data) {
      if (cap.data.empty())
        continue;

      const Size & size =
          cap.camera->GetResolutions()[cap.camera->GetConfiguration().format_index][cap.camera->GetConfiguration().resolution_index];
      cv::Mat as_img(size.height, size.width, CV_8UC3, (void *) cap.data.data());
      cv::cvtColor(as_img, as_img, cv::COLOR_RGB2BGR);
      cv::imshow(cap.camera->GetUniqueId(), as_img);
    }
    cv::waitKey(1);
  }
};

}
}
}

int main(int argc, char * argv[]) {


  gago::io::video::V4lDriver driver;
  driver.Initialize();
  auto cameras = driver.GetCameras();

  if (cameras.empty()) {
//    ROS_ERROR("No cameras to capture");
  }

  if (cameras.size() > 1) {
//    ROS_INFO("Found more than one cameras. Camera count: %i. Will use the first one that successfully opens");
  }

  auto formats = cameras[0]->GetFormats();
  bool motion_jpeg = false;
  for (int i = 0; i < formats.size(); ++i) {
    if (formats[i] == "Motion-JPEG") {
      motion_jpeg = true;
      gago::io::video::CameraSettings settings(cameras[0]);

      settings.config.format_index = i;
      settings.config.name = "WorkingCamera";
      settings.config.resolution_index = 0;
      settings.config.vertical_flip = false;
      settings.config.decode_image = false;
      driver.SetSettings(std::vector<gago::io::video::CameraSettings>{settings});
    }
  }


  /*gago::io::video::V4lDriver driver;
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
  driver.Join();*/
}