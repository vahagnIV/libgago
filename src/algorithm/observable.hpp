//
// Created by vahagn on 1/2/20.
//

#ifndef LIBGAGO_SRC_ALGORITHM_I_OBSERVABLE_H_
#define LIBGAGO_SRC_ALGORITHM_I_OBSERVABLE_H_
#include "observer.hpp"
#include <vector>
#include <algorithm>

namespace gago {
namespace algorithm {

template<typename T>
class Observable {
 public:
  Observable() = default;
  void Register(const Observer<T> *observer) {
    bool was_running = IsRunning();
    if (was_running)
      StopLoop();
    observers_.push_back(observer);
    if(was_running)
      StartLoop();
  }
  void Unregister(const Observer<T> *observer) {
    bool was_running = IsRunning();
    if (was_running)
      StopLoop();
    observers_.erase(std::remove_if(observers_.begin(),
                                    observers_.end(),
                                    [&observer](const std::shared_ptr<T> &ob) { return ob == observer; }));
    if(was_running)
      StartLoop();
  }

  virtual ~Observable(){};
 protected:
  virtual void StopLoop(){};
  virtual void StartLoop(){};
  virtual bool IsRunning(){ return false; }
  void Notify(const std::shared_ptr<T> &data) {
    for (Observer<T> &observer: observers_) {
      observer.Notify(data);
    }
  }
  std::vector<Observer<T> *> observers_;

};

}
}
#endif //LIBGAGO_SRC_ALGORITHM_I_OBSERVABLE_H_
