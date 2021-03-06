#pragma once

#include "event_loop.h"
#include "timer.h"

class App final : public async::CallbackHost
{
public:
  static App& Instance()
  {
    static App app;
    return app;
  }

  bool Init();
  void Start();
  void Stop();

  std::chrono::system_clock::time_point WallTime() const { return wall_time_; }
  EventLoop& GetLoop() { return loop_; }

private:
  App();
  ~App() = default;

  void OnTick();

private:
  EventLoop loop_;
  Timer tick_timer_;
  std::chrono::system_clock::time_point wall_time_;
  std::chrono::steady_clock::time_point tick_time_;
};

#define AppInst App::Instance()
