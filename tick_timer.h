#pragma once

#include <vector>
#include "callback.h"

using TickTimerCallback = async::Callback<void()>;

struct TimerNode
{
  TimerNode* next;

  TickTimerCallback callback;
  size_t expire;
  uint64_t tid;
  uint32_t interval : 30;
  uint32_t periodic : 1;
  uint32_t valid : 1;

  TimerNode()
    : next(nullptr)
    , expire(0)
    , tid(0)
    , interval(0)
    , periodic(0)
    , valid(0)
  {
  }
};

struct TickTimerID
{
  TimerNode* ptr{nullptr};
  uint64_t id{0};

  bool operator==(const TickTimerID& other) const;
  bool IsValid() const;
};

class TickTimerWheel
{
public:
  TickTimerWheel(size_t size, size_t ticks_per_slot);
  void AddNode(TimerNode* node);
  void AddNodes(TimerNode* nodes, size_t current_tick);

  size_t MoveNext();
  TimerNode& CurrentSlot() { return slots_[cursor_]; }
  size_t SlotCount() const { return slots_.size(); }
  size_t WheelTicks() const { return kSlotTicks * slots_.size(); }
  size_t Cursor() const { return cursor_; }

private:
  std::vector<TimerNode> slots_;
  size_t cursor_;
  const size_t kSlotTicks;
};

class TickTimerManager
{
public:
  TickTimerManager(std::initializer_list<size_t> il);
  void RunTick();

  TickTimerID AddPeriodTimer(
    uint32_t interval, const TickTimerCallback& callback);
  TickTimerID AddOneshotTimer(
    uint32_t delay, const TickTimerCallback& callback);
  void RemoveTimer(TickTimerID timer_id);

  size_t MaxTimerTicks() const { return wheels_.rbegin()->WheelTicks(); }

private:
  TickTimerID AddTimer(
    uint32_t interval, const TickTimerCallback& callback, bool is_periodic);
  void AddNode(TimerNode* node);
  void MoveWheel(size_t index);

  TimerNode* MakeNode();
  void RecycleNode(TimerNode* node);

  uint64_t GeneratorTimerId();

private:
  std::vector<TickTimerWheel> wheels_;
  size_t tick_;
  TimerNode recycle_;
  uint64_t last_tid_;
};
