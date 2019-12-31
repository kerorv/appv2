#pragma once

#include "message.h"

struct ShakeHandRequest : public Message
{
  uint64_t from;
  uint64_t to;
};

struct ShakeHandResponse : public Message
{
  uint64_t from;
  int32_t error;
};
