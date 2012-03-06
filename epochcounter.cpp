
// Copyright 2006 Mike Desjardins.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "epochcounter.h"

namespace dragonfly {
  const tickcount_t EpochCounter::TICKS_PER_SECOND = 1000;
  const tickcount_t EpochCounter::TICKS_PER_MINUTE = TICKS_PER_SECOND * 60;
  const tickcount_t EpochCounter::TICKS_PER_HOUR = TICKS_PER_MINUTE * 60;
  const tickcount_t EpochCounter::TICKS_PER_DAY = TICKS_PER_HOUR * 24;
  const tickcount_t EpochCounter::TICKS_PER_WEEK = TICKS_PER_DAY * 7;
  const datecount_t EpochCounter::DAYS_PER_WEEK = 7;
}
