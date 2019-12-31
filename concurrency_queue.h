//************************************************************************
// author:  jianxihua@gmail.com
// created: 2017/03/16
// desc:    concurrent queue
//************************************************************************
#pragma once

#include <deque>
#include <mutex>
#include <vector>

namespace base
{
template <typename Data>
class ConcurrencyQueue
{
public:
  void Put(const Data& data)
  {
    std::lock_guard<std::mutex> guard(mutex_);
    queue_.push_back(data);
  }

  void Put(Data&& data)
  {
    std::lock_guard<std::mutex> guard(mutex_);
    queue_.push_back(std::forward<Data>(data));
  }

  bool Get(Data& data)
  {
    std::lock_guard<std::mutex> guard(mutex_);
    if (queue_.empty())
    {
      return false;
    }

    data = std::move(queue_.front());
    queue_.pop_front();
    return true;
  }

  void Gets(std::vector<Data>& dest)
  {
    std::lock_guard<std::mutex> guard(mutex_);
    for (auto& data : queue_)
    {
      dest.push_back(std::move(data));
    }
    queue_.clear();
  }

  void Swap(std::deque<Data>& dest)
  {
    std::lock_guard<std::mutex> guard(mutex_);
    std::swap(queue_, dest);
  }

  size_t Size() const
  {
    std::lock_guard<std::mutex> guard(mutex_);
    return queue_.size();
  }

private:
  std::deque<Data> queue_;
  mutable std::mutex mutex_;
};
}
