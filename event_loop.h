#pragma once

#include <functional>
#include "concurrency_queue.h"
#include "event_poller.h"
#include "tick_timer.h"

class EventLoopT final
{
  friend class Timer;

public:
  EventLoopT();
  ~EventLoopT() = default;

  void Run();
  void Stop();

  template <typename F>
  void Post(F&& task)
  {
    tasks_.Put(std::forward<F>(task));
    poller_.Set();
  }

  using TickPeriod = std::chrono::duration<int64_t, std::deci>;
  const TickPeriod kOneTickPeriod{1};

  using Handler = std::function<void()>;

private:
  bool stop_action_;
  EventPoller poller_;
  base::ConcurrencyQueue<Handler> tasks_;
  TickTimerManager ttm_;
};
