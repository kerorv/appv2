#pragma once

#if defined(_WIN32)
#include "win_event_poller.h"
#define EventPooler WinEventPooler
#else
#error "unsupport platform"
#endif
