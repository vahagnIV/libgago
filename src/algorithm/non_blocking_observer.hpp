//
// Created by vahagn on 1/2/20.
//

#ifndef LIBGAGO_SRC_ALGORITHM_NON_BLOCKING_OBSERVER_H_
#define LIBGAGO_SRC_ALGORITHM_NON_BLOCKING_OBSERVER_H_
#include "observer.hpp"
#include "concurrentqueue.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>

namespace gago {
namespace algorithm {

template<typename T>
class NonBlockingObserver : public Observer<T> {
 public:
  NonBlockingObserver(int max_queue_count = 1) : max_queue_count_(max_queue_count), cancelled_(false), running_(false) {

  }
  void Start() {
    if (Running())
      return;
    thread_ = new std::thread(&NonBlockingObserver::WorkingLoop, this);
  }

  void Stop() {
    cancelled_ = true;
    thread_->join();
    running_ = false;
    delete thread_;

    // drain the queue
    std::shared_ptr<T> placeholder;
    while (queue_.try_dequeue(placeholder));
  }

  bool Running() {
    return running_;
  }

  void Notify(std::shared_ptr<T> &ptr) override {
    if (!Running())
      return;
    if (queue_.size_approx() < max_queue_count_)
      queue_.enqueue(ptr);
    condition_variable_.notify_one();
  }

  virtual ~NonBlockingObserver() {
    std::cout << "Destroyinh the thread" << std::endl;
    if (Running())
      Stop();

  }
 protected:

  virtual void Work() {
    // Use the queue to process items, e.g.
    // std::shared_ptr<T> r;
    // while (queue_.try_dequeue(r))
    //     std::cout << *r << std::endl;
  }

  void WorkingLoop() {
    running_ = true;
    while (!cancelled_) {
      std::unique_lock<std::mutex> lk(mutex_);
      condition_variable_.wait(lk);
      Work();
    }
    cancelled_ = false;
  }

  std::atomic_bool cancelled_;
  std::atomic_bool running_;
  moodycamel::ConcurrentQueue<std::shared_ptr<T>> queue_;
  std::mutex mutex_;
  std::condition_variable condition_variable_;
  int max_queue_count_;
  std::thread *thread_;

};

}
}
#endif //LIBGAGO_SRC_ALGORITHM_NON_BLOCKING_OBSERVER_H_
