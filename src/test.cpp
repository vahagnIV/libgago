//
// Created by vahagn on 1/2/20.
//

#include <algorithm/non_blocking_observer.hpp>
#include <io/video/linux/v4l_driver.h>
#include "io/video/linux/v4l_camera.h"
//#include "io/video/linux/v4l_driver.h"

class OpenCvPlayer : public gago::algorithm::Observer<std::vector<gago::io::video::Capture>> {
 public:
  OpenCvPlayer(){
    window_names.push_back("cam1");
    window_names.push_back("cam2");

  }

  void Notify(const std::shared_ptr<std::vector<gago::io::video::Capture>> &ptr) override {
    cv::namedWindow(window_names[0], cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
    cv::namedWindow(window_names[1], cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
    for (int i = 0; i < ptr->size(); ++i) {
      cv::imshow(window_names[i], (*ptr)[i].data);
    }
    cv::waitKey(1);
  }
 std::vector<std::string> window_names;

};

int main() {

  gago::io::video::V4lDriver driver;
  driver.Initialize();
  OpenCvPlayer player;
  driver.Register(&player);
  driver.Start();
  driver.Join();


  gago::io::video::V4lCamera *cam = gago::io::video::V4lCamera::Create("/dev/video0");


  cam->PrepareBuffers();
  cam->Grab();
  cv::Mat m;
  cam->Retieve(m);
  cv::imshow("asd", m);
  cv::waitKey();

  std::cout << "sss" << std::endl;
}