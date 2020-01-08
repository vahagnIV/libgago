//
// Created by vahagn on 1/2/20.
//

#include <algorithm/non_blocking_observer.hpp>
#include "io/video/camera.h"
#include "io/video/linux/v4l_driver.h"

int main(){

  gago::io::video::V4lDriver d;
  d.Initialize();

 d.Start();
 std::cout << "sss" << std::endl;
}