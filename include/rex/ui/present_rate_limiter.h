// Copyright 2026 Fable II Recomp contributors.
// Released under the BSD license; see LICENSE in the SDK root.
#pragma once

#include <atomic>
#include <chrono>
#include <cstdint>

namespace rex::ui {

// Caps host presents per second by skipping presents, never by waiting, so the
// guest keeps running at its own rate. Slots sit on a fixed grid so jitter
// doesn't lower the average; a late present restarts the grid instead of
// allowing a burst.
class PresentRateLimiter {
 public:
  using Clock = std::chrono::steady_clock;

  static bool IsEnabled(int32_t frames_per_second) {
    return frames_per_second > 0 && frames_per_second <= kMaxFramesPerSecond;
  }

  bool Allows(int32_t frames_per_second, Clock::time_point now) const {
    return !IsEnabled(frames_per_second) ||
           now.time_since_epoch().count() >= next_present_.load(std::memory_order_relaxed);
  }

  void NotePresent(int32_t frames_per_second, Clock::time_point now) {
    if (!IsEnabled(frames_per_second)) {
      return;
    }
    const auto period = std::chrono::duration_cast<Clock::duration>(
                            std::chrono::duration<double>(1.0 / frames_per_second))
                            .count();
    const auto current = now.time_since_epoch().count();
    auto next = next_present_.load(std::memory_order_relaxed) + period;
    if (next <= current || next > current + period) {
      next = current + period;
    }
    next_present_.store(next, std::memory_order_relaxed);
  }

  static constexpr int32_t kMaxFramesPerSecond = 1000;

 private:
  std::atomic<Clock::rep> next_present_{0};
};

}  // namespace rex::ui
