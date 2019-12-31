#include "input_channel.h"
#include "core_msg.h"

#define READ_BUFFER_BLOCK 4096
#define INIT_READ_BUFFER READ_BUFFER_BLOCK

InputChannel::InputChannel(asio::ip::tcp::socket socket, SessionAddress me)
  : socket_(std::move(socket))
  , timer_(socket_.get_executor())
  , me_(me)
  , peer_(0)
{
  rbuffer_.buffer = reinterpret_cast<char*>(malloc(INIT_READ_BUFFER));
  rbuffer_.capacity = INIT_READ_BUFFER;
  rbuffer_.size = 0;
}

asio::awaitable<SessionAddress> InputChannel::Init()
{
  ShakeHandRequest sh_req;
  co_await asio::async_read(socket_,
    asio::buffer(&sh_req, sizeof(ShakeHandRequest)), asio::use_awaitable);

  ShakeHandResponse sh_rep;
  sh_rep.error = 0;

  if (sh_req.to != me_)
  {
    sh_rep.error = 100;  // error address
  }
  else
  {
    peer_ = sh_req.from;
  }

  co_await asio::async_write(socket_,
    asio::buffer(&sh_rep, sizeof(ShakeHandResponse)), asio::use_awaitable);

  if (peer_ == 0)
  {
    // let response arrive
    timer_.expires_after(std::chrono::seconds{10});
    co_await timer_.async_wait(asio::use_awaitable);
  }

  co_return peer_;
}

asio::awaitable<void> InputChannel::Start()
{
  for (;;)
  {
    // read message header
    co_await asio::async_read(socket_,
      asio::buffer(rbuffer_.buffer, sizeof(Message)), asio::use_awaitable);
    Message* header = reinterpret_cast<Message*>(rbuffer_.buffer);
    if (header->size < sizeof(Message))
    {
      // protocol error
      Close();
      co_return;
    }

    if (header->size > sizeof(Message))
    {
      // read message body
      if (rbuffer_.capacity < header->size)
      {
        // adjust buffer capacity
        uint16_t new_capacity =
          (header->size + READ_BUFFER_BLOCK - 1) / READ_BUFFER_BLOCK;
        char* new_buff = reinterpret_cast<char*>(malloc(new_capacity));
        memmove(rbuffer_.buffer, new_buff, sizeof(Message));
        free(rbuffer_.buffer);
        rbuffer_.buffer = new_buff;
        rbuffer_.capacity = new_capacity;
      }

      co_await asio::async_read(socket_,
        asio::buffer(
          rbuffer_.buffer + sizeof(Message), header->size - sizeof(Message)),
        asio::use_awaitable);
    }

    // post nessage to net engine
    // TODO
  }
}

void InputChannel::Close()
{
  socket_.close();
}
