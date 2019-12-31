#pragma once

#include <deque>
#include <memory>
#include <string>
#include "thirtyparty/asio/asio.hpp"
#include "message.h"
#include "session_address.h"

class OutputChannel : public std::enable_shared_from_this<OutputChannel>
{
public:
  OutputChannel(
    asio::io_context& ioctx, SessionAddress me, SessionAddress peer);

  asio::awaitable<void> Start(const asio::ip::tcp::endpoint& target);
  void SendMsg(const Message& msg);
  void Close();

private:
  asio::awaitable<bool> Init(const asio::ip::tcp::endpoint& target);
  asio::awaitable<void> Send();

private:
  asio::io_context& ioctx_;
  std::unique_ptr<asio::ip::tcp::socket> socket_;
  SessionAddress me_;
  SessionAddress peer_;
  asio::steady_timer timer_;
  std::deque<std::string> out_msgs_;
  bool connected_;
};
