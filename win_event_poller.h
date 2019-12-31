#pragma once

#include <cstdint>

class EventPoller final
{
public:
  EventPoller(uint32_t interval);
  ~EventPoller();

  enum
  {
    EP_Error = -1,
    EP_Timeout = 0,
    EP_Event = 1,
    EP_Timer = 2,
  };

  int Poll(uint32_t timeout = UINT32_MAX);
  void Set();

private:
  void* event_;
  void* timer_;
};
