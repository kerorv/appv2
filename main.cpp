#include <ctime>
#include <iostream>
#include "app.h"
#include "event_poller.h"

std::chrono::system_clock::time_point start_time;
static uint32_t second_tick = 0;

void PrintTime(std::chrono::system_clock::time_point tp)
{
  std::time_t tt = std::chrono::system_clock::to_time_t(tp);
  auto tp1 = std::chrono::system_clock::from_time_t(tt);
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(tp - tp1);
  auto tm = localtime(&tt);
  printf("%d-%d-%d %d:%d:%d.%I64d", tm->tm_year + 1900, tm->tm_mon + 1,
    tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec, ms.count());
}

void OnTimer()
{
  std::cout << "Start time: ";
  PrintTime(start_time);
  std::cout << std::endl;

  std::cout << "App wall time: ";
  PrintTime(AppInst.WallTime());
  std::cout << std::endl;

  std::cout << "System time: ";
  PrintTime(std::chrono::system_clock::now());
  std::cout << std::endl;

  std::cout << std::endl;

  second_tick = 0;
}

void OnTimer2()
{
  ++second_tick;
  printf("%u\n", second_tick);
}

int main(int argc, char* argv[])
{
  start_time = std::chrono::system_clock::now();
  std::cout << "Start time: ";
  PrintTime(start_time);
  std::cout << "\n" << std::endl;

  AppTimer timer;
  auto callback = async::Bind<void()>(&OnTimer);
  using namespace std::chrono_literals;
  timer.StartRepeat(10s, callback);

  AppTimer timer2;
  auto callback2 = async::Bind<void()>(&OnTimer2);
  timer2.StartRepeat(1s, callback2);

  App::Instance().Start();

  return 0;
}
