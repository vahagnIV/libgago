//
// Created by vahagn on 1/2/20.
//

#include <algorithm/non_blocking_observer.hpp>
#include "io/video/camera.h"
#include "io/video/linux/v4l_driver.h"

int main(){

  gago::io::video::V4lDriver d;
  d.Initialize();

  gago::algorithm::NonBlockingObserver<int> observer(1);
  observer.Start();
  for (int i = 0; i < 1000; ++i) {
    std::shared_ptr<int> m = std::make_shared<int>(i);
    std::this_thread::sleep_for(std::chrono::milliseconds(15));
    observer.Notify(m);
  }
}