#include <ctime>
#include <iostream>
#include "app.h"
#include "event_poller.h"

std::chrono::system_clock::time_point start_time;

void PrintTime(std::chrono::system_clock::time_point tp)
{
  std::time_t tt = std::chrono::system_clock::to_time_t(tp);
  auto tp1 = std::chrono::system_clock::from_time_t(tt);
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(tp - tp1);
  auto tm = localtime(&tt);
  printf("%d-%d-%d %d:%d:%d.%I64d", tm->tm_year + 1900, tm->tm_mon + 1,
    tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec, ms.count());
}

void OnTimer(TickTimerID tid)
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
}

int main(int argc, char* argv[])
{
  start_time = std::chrono::system_clock::now();
  std::cout << "Start time: ";
  PrintTime(start_time);
  std::cout << "\n" << std::endl;

  AppTimer timer;
  auto callback = async::Bind<void(TickTimerID)>(&OnTimer);
  using namespace std::chrono_literals;
  timer.StartRepeat(10s, callback);

  App::Instance().Start();

  return 0;
}
