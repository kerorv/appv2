#include "win_event_poller.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

EventPoller::EventPoller(uint32_t interval)
  : event_(::CreateEventA(NULL, FALSE, FALSE, NULL))
  , timer_(::CreateWaitableTimerA(NULL, FALSE, NULL))
{
  LARGE_INTEGER due_time;
  due_time.QuadPart = 0;
  ::SetWaitableTimer(timer_, &due_time, interval, NULL, NULL, FALSE);
}

EventPoller::~EventPoller()
{
  ::CloseHandle(event_);
  ::CloseHandle(timer_);
}

int EventPoller::Poll(uint32_t timeout)
{
  HANDLE waitobjs[2] = {timer_, event_};
  DWORD ret = ::WaitForMultipleObjects(2, waitobjs, FALSE, timeout);
  switch (ret)
  {
    case WAIT_OBJECT_0:
    {
      // timer, then check event
      if (WAIT_OBJECT_0 == ::WaitForSingleObject(event_, 0))
      {
        return EP_Timer | EP_Event;
      }

      return EP_Timer;
    }
    break;
    case (WAIT_OBJECT_0 + 1):
    {
      // event
      return EP_Event;
    }
    break;
    case WAIT_TIMEOUT:
    {
      return EP_Timeout;
    }
    break;
    case WAIT_FAILED:
    default:
    {
      return EP_Error;
    }
    break;
  }
}

void EventPoller::Set()
{
  ::SetEvent(event_);
}
