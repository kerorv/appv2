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
  tick_time_ = std::chrono::steady_clock::now();
  auto callback = async::Bind<void()>(&App::OnTick, this);
  tick_timer_.StartRepeat(loop_.kOneTickPeriod, callback);
  loop_.Run();
}

void App::Stop()
{
  return loop_.Stop();
}

void App::OnTick()
{
  auto pass_duration = std::chrono::duration_cast<std::chrono::milliseconds>(
    std::chrono::steady_clock::now() - tick_time_);
  if (pass_duration <= loop_.kOneTickPeriod)
  {
    pass_duration = loop_.kOneTickPeriod;
  }

  tick_time_ += pass_duration;
  wall_time_ += pass_duration;
}
