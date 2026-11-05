// Copyright 2026 Fable II Recomp contributors.
// Released under the BSD license; see LICENSE in the SDK root.
#include <rex/graphics/precise_sleeper.h>
#include <thread>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

namespace rex::graphics {

PreciseSleeper::~PreciseSleeper() {
#ifdef _WIN32
  if (timer_) CloseHandle(static_cast<HANDLE>(timer_));
#endif
}

void PreciseSleeper::SleepFor(std::chrono::nanoseconds duration) {
  if (duration.count() <= 0) return;
#ifdef _WIN32
  if (!timer_attempted_) {
    timer_attempted_ = true;
    timer_ = CreateWaitableTimerExW(nullptr, nullptr, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
                                    TIMER_MODIFY_STATE | SYNCHRONIZE);
  }
  if (timer_) {
    LARGE_INTEGER due;
    due.QuadPart = -((duration.count() + 99) / 100);  // Relative 100 ns, rounded up.
    if (SetWaitableTimer(static_cast<HANDLE>(timer_), &due, 0, nullptr, nullptr, FALSE) &&
        WaitForSingleObject(static_cast<HANDLE>(timer_), INFINITE) == WAIT_OBJECT_0) {
      return;
    }
  }
#endif
  std::this_thread::sleep_for(duration);
}

}  // namespace rex::graphics
