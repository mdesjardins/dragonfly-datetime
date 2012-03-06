#ifndef __DURATION_H__
#define __DURATION_H__

// Copyright 2006 Mike Desjardins.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "epochcounter.h"
#include "datetypes.h"

namespace dragonfly {

class Duration : public EpochCounter
{
  public:
    Duration(const EpochCounter& parent) 
    { 
      EpochCounter::ticks(parent.ticks());
      EpochCounter::days(parent.days());
    }

  public:
    Duration& operator= (const EpochCounter& parent)
    {
      EpochCounter::ticks(parent.ticks());
      EpochCounter::days(parent.days());       
      return *this;
    }
    
  protected:
    Duration(tickcount_t init) { ticks(init); }
    
  public:
    const int weeks() const { return EpochCounter::days() / DAYS_PER_WEEK; }
    const int days() const { return EpochCounter::days(); }
    const int hours() const { 
      return (EpochCounter::days() * TICKS_PER_DAY + ticks()) / TICKS_PER_HOUR; 
    }
    const int minutes() const { 
      return (EpochCounter::days() * TICKS_PER_DAY + ticks()) / TICKS_PER_MINUTE; 
    }
    const int seconds() const { 
      return (EpochCounter::days() * TICKS_PER_DAY + ticks()) / TICKS_PER_SECOND; 
    }
    const int subseconds() const { 
      return (EpochCounter::days() * TICKS_PER_DAY + ticks()) % TICKS_PER_SECOND; 
    }
};

//-----------------------------------------------------
class Weeks : public Duration
{
  public:
    Weeks(int weeks) : Duration(weeks * TICKS_PER_WEEK) {}
};

//-----------------------------------------------------
class Days : public Duration
{
  public:
    Days(int days) : Duration(days * TICKS_PER_WEEK) {}  
};

//-----------------------------------------------------
class Hours : public Duration
{
  public:
    Hours(int hours) : Duration(hours * TICKS_PER_HOUR) {}    
};

//-----------------------------------------------------
class Minutes : public Duration
{
  public:
    Minutes(int minutes) : Duration(minutes * TICKS_PER_MINUTE) {}  
};

//-----------------------------------------------------
class Seconds : public Duration
{
  public:
    Seconds(int seconds) : Duration(seconds * TICKS_PER_SECOND) {}
};

//-----------------------------------------------------
class SubSeconds : public Duration
{
  public:
    SubSeconds(int subseconds) : Duration(subseconds) {}
};

} // namespace dragonfly

#endif //__DURATION_H__
