#include "timer.h"
#include "app.h"
#include "event_loop.h"

////////////////////////////////////////////////////////////////////
// Timer
Timer::Timer(EventLoopT& loop)
  : loop_(loop)
{
}

Timer::~Timer()
{
  Stop();
}

void Timer::StartRepeat(
  std::chrono::milliseconds interval, const TickTimerCallback& callback)
{
  Stop();

  uint32_t interval_tick = interval / loop_.kOneTickPeriod;
  if (interval_tick == 0 || interval_tick > loop_.ttm_.MaxTimerTicks())
  {
    throw std::runtime_error("Invalid repeat ticktimer interval");
  }

  tid_ = loop_.ttm_.AddPeriodTimer(interval_tick, callback);
}

void Timer::StartOnce(
  std::chrono::milliseconds delay, const TickTimerCallback& callback)
{
  Stop();

  uint32_t delay_tick = delay / loop_.kOneTickPeriod;
  if (delay_tick == 0 || delay_tick > loop_.ttm_.MaxTimerTicks())
  {
    throw std::runtime_error("Invalid oneshot ticktimer interval");
  }

  tid_ = loop_.ttm_.AddOneshotTimer(delay_tick, callback);
}

void Timer::Stop()
{
  if (tid_.IsValid())
  {
    loop_.ttm_.RemoveTimer(tid_);
  }

  tid_.ptr = nullptr;
  tid_.id = 0;
}

////////////////////////////////////////////////////////////////////
// AppTimer
AppTimer::AppTimer()
  : Timer(AppInst.GetLoop())
{
}
