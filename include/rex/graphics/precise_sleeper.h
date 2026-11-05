// Copyright 2026 Fable II Recomp contributors.
// Released under the BSD license; see LICENSE in the SDK root.
#pragma once

#include <chrono>

namespace rex::graphics {

// Sub-millisecond sleeps for one thread. A plain Sleep on Windows rounds up to
// the system timer tick (15.6 ms by default), so this uses a high-resolution
// waitable timer and falls back to a plain sleep where none is available.
class PreciseSleeper {
 public:
  PreciseSleeper() = default;
  ~PreciseSleeper();
  PreciseSleeper(const PreciseSleeper&) = delete;
  PreciseSleeper& operator=(const PreciseSleeper&) = delete;
  void SleepFor(std::chrono::nanoseconds duration);

 private:
  void* timer_ = nullptr;
  bool timer_attempted_ = false;
};

}  // namespace rex::graphics
