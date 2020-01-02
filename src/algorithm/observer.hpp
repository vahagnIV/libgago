//
// Created by vahagn on 1/2/20.
//

#ifndef LIBGAGO_SRC_ALGORITHM_OBSERVER_H_
#define LIBGAGO_SRC_ALGORITHM_OBSERVER_H_
#include <memory>

namespace gago {
namespace algorithm {

template<typename T>

class Observer {
 public:
  virtual void Notify(std::shared_ptr<T> &) = 0;
  virtual ~Observer() = default;

};

}
}
#endif //LIBGAGO_SRC_ALGORITHM_OBSERVER_H_
