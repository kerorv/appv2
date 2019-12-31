#include "output_channel.h"
#include "core_msg.h"

OutputChannel::OutputChannel(
  asio::io_context& ioctx, SessionAddress me, SessionAddress peer)
  : ioctx_(ioctx)
  , me_(me)
  , peer_(peer)
  , timer_(ioctx)
  , connected_(false)
{
}

asio::awaitable<void> OutputChannel::Start(
  const asio::ip::tcp::endpoint& target)
{
  for (bool success = false; success = co_await Init(target);)
  {
    if (success)
    {
      break;
    }

    timer_.expires_after(std::chrono::seconds{5});
    co_await timer_.async_wait(asio::use_awaitable);
  }

  asio::co_spawn(
    ioctx_, [self = shared_from_this()] { return self->Send(); },
    asio::detached);
}

void OutputChannel::SendMsg(const Message& msg)
{
  std::string buff(
    reinterpret_cast<char*>(const_cast<Message*>(&msg)), msg.size);
  out_msgs_.push_back(std::move(buff));
  if (connected_ && out_msgs_.size() == 1)
  {
    asio::co_spawn(
      ioctx_, [self = shared_from_this()] { return self->Send(); },
      asio::detached);
  }
}

void OutputChannel::Close()
{
  if (socket_)
  {
    if (socket_->is_open())
    {
      socket_->close();
    }

    socket_.reset();
  }
}

asio::awaitable<bool> OutputChannel::Init(const asio::ip::tcp::endpoint& target)
{
  for (;;)
  {
    socket_ = std::make_unique<asio::ip::tcp::socket>(ioctx_);
    std::error_code ec;
    co_await socket_->async_connect(
      target, asio::redirect_error(asio::use_awaitable, ec));
    if (!ec)
    {
      break;
    }

    printf("connect error: %s\n", ec.message().c_str());
    socket_->close();
    socket_.reset();

    timer_.expires_after(std::chrono::seconds{1});
    co_await timer_.async_wait(asio::use_awaitable);
  }

  ShakeHandRequest sh_req;
  sh_req.from = me_;
  sh_req.to = peer_;
  co_await asio::async_write(*socket_,
    asio::buffer(&sh_req, sizeof(ShakeHandRequest)), asio::use_awaitable);

  ShakeHandResponse sh_rep;
  co_await asio::async_read(*socket_,
    asio::buffer(&sh_rep, sizeof(ShakeHandResponse)), asio::use_awaitable);

  connected_ = true;

  co_return(sh_rep.error != 0);
}

asio::awaitable<void> OutputChannel::Send()
{
  while (!out_msgs_.empty())
  {
    std::string& msg = out_msgs_.front();
    co_await asio::async_write(
      *socket_, asio::buffer(msg), asio::use_awaitable);
    out_msgs_.pop_front();
  }
}
