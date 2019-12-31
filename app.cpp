#include "app.h"

App::App()
  : tick_timer_(loop_)
{
}

bool App::Init()
{
  return true;
}

void App::Start()
{
  wall_time_ = std::chrono::system_clock::now();
  auto callback = async::Bind<void(TickTimerID)>(&App::OnTick, this);
  tick_timer_.StartRepeat(loop_.kOneTickPeriod, callback);
  loop_.Run();
}

void App::Stop()
{
  return loop_.Stop();
}

void App::OnTick(TickTimerID)
{
  wall_time_ += loop_.kOneTickPeriod;
}
