#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
public:
  explicit ThreadPool(size_t n = std::thread::hardware_concurrency());
  ~ThreadPool();

  void enqueue(const std::function<void()> &func);

  void wait();
  void stop();

private:
  void loop();

  std::mutex _mtx;
  std::condition_variable _mtx_cond, _done;
  std::queue<std::function<void()>> _queue;
  std::vector<std::thread> _workers;
  bool _stop;
  size_t _pending;
};
