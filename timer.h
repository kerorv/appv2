#pragma once

#include <chrono>
#include "tick_timer.h"

class EventLoop;
class Timer
{
public:
  Timer(EventLoop& loop);
  ~Timer();

  void StartRepeat(std::chrono::milliseconds interval, const TickTimerCallback& callback);
  void StartOnce(std::chrono::milliseconds interval, const TickTimerCallback& callback);
  void Stop();
  
  TickTimerID GetID() const { return tid_; }

private:
  EventLoop& loop_;
  TickTimerID tid_;
};

class AppTimer final: public Timer
{
public:
  AppTimer();
};
