#pragma once

#include <cstdint>

struct Message
{
  Message(uint16_t msg_size, uint16_t msg_type)
    : size(msg_size)
    , type(msg_type)
  {
  }

  Message()
    : size(0)
    , type(0)
  {
  }

  uint16_t size;
  uint16_t type;
};
