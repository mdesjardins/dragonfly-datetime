#ifndef __EPOCHCOUNTER_H__
#define __EPOCHCOUNTER_H__

// Copyright 2006 Mike Desjardins.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "datetypes.h"

namespace dragonfly {

class EpochCounter {
  public:
    EpochCounter() : m_days(0), m_ticks(0) {}
    EpochCounter(const EpochCounter& other) : 
      m_days(other.m_days), m_ticks(other.m_ticks) {}
    virtual ~EpochCounter() {};
     
  public:
    const EpochCounter& operator= (const EpochCounter& other)
    { m_days = other.m_days; m_ticks = other.m_ticks; return *this; }
  
  public:
    const bool operator< (const EpochCounter& other);
    const bool operator<= (const EpochCounter& other);
    const bool operator> (const EpochCounter& other);
    const bool operator>= (const EpochCounter& other);

  public:
    static const tickcount_t TICKS_PER_SECOND; 
    static const tickcount_t TICKS_PER_MINUTE;
    static const tickcount_t TICKS_PER_HOUR;
    static const tickcount_t TICKS_PER_DAY;
    static const tickcount_t TICKS_PER_WEEK;
    static const datecount_t DAYS_PER_WEEK;
    
  public: 
    void operator-= (const EpochCounter& other);
    void operator+= (const EpochCounter& other);

  public:
    const datecount_t& days() const { return m_days; }
    void days(const datecount_t& days) { m_days = days; }
    const tickcount_t& ticks() const { return m_ticks; }
    void ticks(const tickcount_t& ticks) { m_ticks = ticks; }
 
  private:
    datecount_t m_days;
    tickcount_t m_ticks;
};

inline const bool EpochCounter::operator< (const EpochCounter& other)
{ 
  return (m_days == other.m_days ? 
    m_ticks < other.m_ticks : m_days < other.m_days); 
}
inline const bool EpochCounter::operator<= (const EpochCounter& other)
{ 
  return (m_days == other.m_days ? 
    m_ticks <= other.m_ticks : m_days <= other.m_days); 
}
inline const bool EpochCounter::operator> (const EpochCounter& other)
{ 
  return (m_days == other.m_days ? 
    m_ticks > other.m_ticks : m_days > other.m_days); 
}
inline const bool EpochCounter::operator>= (const EpochCounter& other)
{ 
  return (m_days == other.m_days ? 
    m_ticks >= other.m_ticks : m_days >= other.m_days); 
}

//----------------------------------------------------------------------------
// Increment/Decrement
inline void EpochCounter::operator+= (const EpochCounter& other) 
{ 
  m_ticks += other.m_ticks;
  if (m_ticks >= TICKS_PER_DAY)
  {
    ++m_days;
    m_ticks = m_ticks + other.m_ticks - TICKS_PER_DAY;
  }
  m_days += other.m_days;
}
inline void EpochCounter::operator-= (const EpochCounter& other) 
{ 
  if (m_ticks > other.m_ticks)
    m_ticks -= other.m_ticks;   
  else
  {
    --m_days;
    m_ticks = TICKS_PER_DAY - other.m_ticks + m_ticks;
  }
  m_days -= other.m_days;
}

//----------------------------------------------------------------------------
// Globally scoped addition/subtraction operators, defined in terms of member
// functions.
inline EpochCounter operator+ (const EpochCounter& lhs, const EpochCounter& rhs)
{
  EpochCounter temp = lhs;
  temp += rhs;
  return temp;
}
inline EpochCounter operator- (const EpochCounter& lhs, const EpochCounter& rhs)
{
  EpochCounter temp = lhs;
  temp -= rhs;
  return temp;  
}
 
} // namespace dragonfly

#endif //__EPOCHCOUNTER_H__
