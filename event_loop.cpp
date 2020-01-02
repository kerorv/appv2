#include "event_loop.h"

EventLoop::EventLoop()
  : poller_(
      std::chrono::duration_cast<std::chrono::milliseconds>(kOneTickPeriod)
        .count())
  , stop_action_(false)
  , ttm_{600, 60, 24}
{
}

void EventLoop::Run()
{
  stop_action_ = false;
  std::deque<Handler> handlers;

  while (!stop_action_)
  {
    auto ret = poller_.Poll();
    if (ret & EventPoller::EP_Event)
    {
      handlers.clear();
      tasks_.Swap(handlers);

      for (const auto& handler : handlers)
      {
        handler();
      }
    }

    if (ret & EventPoller::EP_Timer)
    {
      ttm_.RunTick();
    }
  }
}

void EventLoop::Stop()
{
  Post([this]() { stop_action_ = true; });
}
