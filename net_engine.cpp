#include "net_engine.h"
#include <thread>

NetEngine::NetEngine()
  : my_address_(0)
{
}

bool NetEngine::Init(const NetConfig& config)
{
  if (!config.cc && !config.local)
  {
    return false;
  }

  config_ = config;

  if (config.cc)
  {
    // call rpc to get local bind address
    // asio::co_spawn(ioctx_, [this]() { InternalInit(config_.cc.value()); });
  }

  if (config.local)
  {
    // bind local address and listen
    acceptor_ =
      std::make_unique<asio::ip::tcp::acceptor>(ioctx_, config.local.value());
    asio::co_spawn(ioctx_, [this] { return Listen(); }, asio::detached);
  }

  std::thread([this] { ioctx_.run(); }).detach();

  return true;
}

asio::awaitable<void> NetEngine::Listen()
{
  for (;;)
  {
    auto channel = std::make_shared<InputChannel>(
      co_await acceptor_->async_accept(asio::use_awaitable), my_address_);

    SessionAddress peer_address = co_await channel->Init();
    if (peer_address == 0)
    {
      channel->Close();
      continue;
    }

    if (auto iter = input_channels_.find(peer_address);
        iter != input_channels_.end())
    {
      iter->second->Close();
    }

    input_channels_.emplace(peer_address, channel);
    asio::co_spawn(
      ioctx_, [channel] { return channel->Start(); }, asio::detached);
  }
}

void NetEngine::SendMsg(SessionAddress to, const Message& msg)
{
  if (auto iter = output_channels_.find(to); iter != output_channels_.end())
  {
    iter->second->SendMsg(msg);
    return;
  }

  asio::ip::tcp::endpoint target;
  if (auto iter = route_table_.find(to); iter == route_table_.end())
  {
    // update route table from cc to get target
    // TODO
  }

  auto channel = std::make_shared<OutputChannel>(ioctx_, my_address_, to);
  output_channels_[to] = channel;

  channel->SendMsg(msg);

  asio::co_spawn(ioctx_, [channel, target] { return channel->Start(target); },
    asio::detached);
}
