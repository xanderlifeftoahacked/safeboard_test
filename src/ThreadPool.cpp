#include "ThreadPool.hpp"

ThreadPool::ThreadPool(size_t n) : _stop(false), _pending(0) {
  if (n == 0)
    n = 4;
  for (size_t i = 0; i < n; ++i)
    _workers.emplace_back([this] { loop(); });
}

ThreadPool::~ThreadPool() { stop(); }

void ThreadPool::enqueue(const std::function<void()> &func) {
  {
    std::lock_guard<std::mutex> lk(_mtx);
    _queue.emplace(std::move(func));
    ++_pending;
  }
  _mtx_cond.notify_one();
}

void ThreadPool::wait() {
  std::unique_lock<std::mutex> lk(_mtx);
  _done.wait(lk, [this] { return _pending == 0; });
}

void ThreadPool::stop() {
  {
    std::lock_guard<std::mutex> lk(_mtx);
    _stop = true;
  }
  _mtx_cond.notify_all();
  for (auto &t : _workers)
    if (t.joinable())
      t.join();
  _workers.clear();
}

void ThreadPool::loop() {
  for (;;) {
    std::function<void()> job;
    {
      std::unique_lock<std::mutex> lk(_mtx);
      _mtx_cond.wait(lk, [this] { return _stop || !_queue.empty(); });
      if (_stop && _queue.empty())
        return;
      job = std::move(_queue.front());
      _queue.pop();
    }
    job();
    {
      std::lock_guard<std::mutex> lk(_mtx);
      if (--_pending == 0)
        _done.notify_all();
    }
  }
}
