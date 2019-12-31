#pragma once

#include "thirtyparty/asio/asio.hpp"
#include "session_address.h"

class InputChannel
{
public:
  InputChannel(asio::ip::tcp::socket socket, SessionAddress me);

  asio::awaitable<SessionAddress> Init();
  asio::awaitable<void> Start();
  void Close();

private:
  asio::ip::tcp::socket socket_;
  asio::steady_timer timer_;
  SessionAddress me_;
  SessionAddress peer_;
  struct ReadBuffer
  {
    char* buffer;
    uint16_t size;
    uint16_t capacity;
  } rbuffer_;
};
