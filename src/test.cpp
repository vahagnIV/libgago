//
// Created by vahagn on 1/2/20.
//

#include <algorithm/non_blocking_observer.hpp>
#include "io/video/linux/v4l_camera.h"
//#include "io/video/linux/v4l_driver.h"

int main(){

  gago::io::video::V4lCamera* cam = gago::io::video::V4lCamera::Create("/dev/video0");

 std::cout << "sss" << std::endl;
}