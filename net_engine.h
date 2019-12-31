#pragma once

#include <map>
#include <memory>
#include <optional>
#include "thirtyparty/asio/asio.hpp"
#include "input_channel.h"
#include "message.h"
#include "output_channel.h"
#include "session_address.h"

struct NetConfig
{
  std::optional<asio::ip::tcp::endpoint> cc;
  std::optional<asio::ip::tcp::endpoint> local;
};

class NetEngine
{
public:
  NetEngine();

  bool Init(const NetConfig& config);
  void SendMsg(SessionAddress to, const Message& msg);

private:
  asio::awaitable<void> Listen();

private:
  SessionAddress my_address_;
  asio::io_context ioctx_;
  NetConfig config_;

  std::unique_ptr<asio::ip::tcp::acceptor> acceptor_;
  std::map<SessionAddress, std::shared_ptr<InputChannel>> input_channels_;
  std::map<SessionAddress, std::shared_ptr<OutputChannel>> output_channels_;

  std::map<SessionAddress, asio::ip::tcp::endpoint> route_table_;
};
