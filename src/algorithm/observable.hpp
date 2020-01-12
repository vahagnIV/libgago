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
  virtual void Register(Observer<T> *observer) = 0;
  virtual ~Observable(){};
 protected:
  void Notify(const std::shared_ptr<T> &data) {
    for (Observer<T> *observer: observers_) {
      observer->Notify(data);
    }
  }
  std::vector<Observer<T> *> observers_;

};

}
}
#endif //LIBGAGO_SRC_ALGORITHM_I_OBSERVABLE_H_
