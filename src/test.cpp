//
// Created by vahagn on 1/2/20.
//

#include <algorithm/non_blocking_observer.hpp>
#include <io/video/linux/v4l_driver.h>
#include "io/video/linux/v4l_camera.h"
//#include "io/video/linux/v4l_driver.h"

class OpenCvPlayer : public gago::io::video::CameraWatcher {
 public:
  OpenCvPlayer() {

  }
  void SetCameras(const std::vector<const gago::io::video::CameraMeta *> & vector) override {
    window_names.resize(0);
    std::string rnd = std::to_string( std::rand());
    for (int i = 0; i < vector.size(); ++i) {
//      cv::namedWindow(vector[i]->GetName() +rnd, cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
      window_names.push_back(vector[i]->GetName()+rnd);
    }
  }

  void Notify(const std::shared_ptr<std::vector<gago::io::video::Capture>> & ptr) override {
    for (int i = 0; i < ptr->size(); ++i) {
      cv::imshow(window_names[i], (*ptr)[i].data);
      std::cout << (*ptr)[i].data.cols << "x" << (*ptr)[i].data.rows << std::endl;
    }
    cv::waitKey(1);
  }
  std::vector<std::string> window_names;
  ~OpenCvPlayer(){
    for (int i = 0; i < window_names.size(); ++i) {
      std::cout<< std::endl<< std::endl<<"======================================"<< std::endl<< std::endl;
      cv::destroyAllWindows();//(window_names[i]);
    }
  }

};

int main() {

  {
    gago::io::video::V4lDriver driver;

    driver.Initialize();
    {
      OpenCvPlayer player;
      driver.Register(&player);
      driver.Start();
      std::this_thread::sleep_for(std::chrono::seconds(5));
      driver.Stop();
      driver.UnRegister(&player);
    }
  }

  gago::io::video::V4lCamera *cam = gago::io::video::V4lCamera::Create("/dev/video0");

  cam->PrepareBuffers();
  cam->Grab();
  cv::Mat m;
  cam->Retieve(m);
  cv::imshow("asd", m);
  cv::waitKey();

  std::cout << "sss" << std::endl;
}