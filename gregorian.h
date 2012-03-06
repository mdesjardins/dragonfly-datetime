#ifndef __GREGORIAN_H__
#define __GREGORIAN_H__

// Copyright 2006 Mike Desjardins.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "epochcounter.h"
#include "dateexception.h"
#include <vector>
#include <exception>
#include <ctime>

namespace dragonfly {

//-----------------------------------------------------------------------------
class Gregorian : public EpochCounter {
  public:
    Gregorian() { /*EpochCounter::EpochCounter();*/ }
    Gregorian(int year, int month, int day) { set(year,month,day); }
    Gregorian(int year, int month, int day, int hour, int minute, int second) {
       set(year,month,day); 
       time(hour, minute, second); 
    }
    Gregorian(const struct tm ts) { 
      set(ts.tm_year, ts.tm_mon, ts.tm_mday); 
      time(ts.tm_hour, ts.tm_min, ts.tm_sec);
    }
    virtual ~Gregorian() {};

  public:
    const int year() const;
    const int month() const;
    const int day() const;
    const int dayOfYear() const;
    const int dayOfWeek() const;

  public:
    const int hour() const;
    const int minute() const;
    const int second() const;
    const int subSecond() const;
  
  public:
    const struct tm getTimeStruct() const;
  
  public:
    //void SetYear(const int year);
    //void SetMonth(const int month);
    //void SetDay(const int day);
    void set(const int y, const int m, const int d);
    void time(const int h, const int m, const int d);

  public:
    //void setLastDayOfMonth();
    void setMidnight();
              
  private:
    static const int c_daycount_a[14];
    static const int c_leapdaycount_a[14];
    static const std::vector<int> c_daycount;
    static const std::vector<int> c_leapdaycount;

    static const int c_lastday_a[12];
    static const int c_leaplastday_a[12];
    static const std::vector<int> c_lastday;
    static const std::vector<int> c_leaplastday;
              
  protected:
    // Is given year a leap year?
    const bool isLeapYear(const int y) const
    { return (y>0) && !(y%4) && ( (y%100) || !(y%400) ); }
    
    // Count the number of leap years from epoch to Jan 1st of year provided.
    const int countLeaps(const int y) const
    { return (y-1)/4 - (y-1)/100 + (y-1)/400; }
    
    // Count the number of days between epoch and Jan 1st of year provided.
    const int countDays(const int y) const
    { return y*365 + countLeaps(y); }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline const int Gregorian::year() const
{ 
  const static int LEAPS_PER_CENTURY = 24;
  const static int DAYS_PER_400 = 400*365 + 4*LEAPS_PER_CENTURY + 1;
  const static int DAYS_PER_100 = 100*365 + LEAPS_PER_CENTURY;
  const static int DAYS_PER_4   = 4*365   + 1;
 
  // There are a couple optimization paths below, one for dates
  // between Jan 1 1900 and Dec 31 1999 (inclusive), and one for
  // dates between Jan 1 2000 and Dec 31 2099 (inclusive).  The
  // theory is that the class will be used for dates in these
  // ranges the most often... the optimized code path for each 
  // hundred year range bypasses a bunch of costly modulo and 
  // divide instructions.
  //
  const static int EPOCH_1900 = 693960;
  const static int EPOCH_2000 = 730484;
  const static int EPOCH_2100 = 767009;
  
  datecount_t x, a, b, c;
  if (days() >= EPOCH_2000 && days() < EPOCH_2100)
  {
    // Optimized for 2000 -> 2100. 
    x = 5; a = 0; 
    b = (days() - EPOCH_2000 - 1)/DAYS_PER_4;
    c = (days() - EPOCH_2000 - 1) - b*DAYS_PER_4;
  }
  else if (days() >= EPOCH_1900 && days() < EPOCH_2000)
  {
    // Optimized for 1900 -> 2000.
    x = 4; a = 3; 
    b = (days() - EPOCH_1900)/DAYS_PER_4;
    c = (days() - EPOCH_1900) - b*DAYS_PER_4;
  }
  else
  {
    // Standard, un-optimized, works-every-time version.  
    x = days(); 
    a = x%DAYS_PER_400; x/=DAYS_PER_400; // whole 400yr intervals
    b = a%DAYS_PER_100; a/=DAYS_PER_100; // whole 100yr intervals
    c = b%DAYS_PER_4;   b/=DAYS_PER_4;   // whole 4 yr intervals
  }
  
  int tentative = 400*x + 100*a + 4*b;
  if (isLeapYear(tentative) && (c < 365))
    return tentative;
  else if (isLeapYear(tentative))
    return tentative + 1 + (c-365)/365;
  else
    return tentative + c/365;
}

//-----------------------------------------------------------------------------
inline const int Gregorian::month() const
{
  const std::vector<int>& month_days =
    (isLeapYear(year()) ? c_leapdaycount : c_daycount);
  
  // guess is trying to predict month where 0=Jan... need to
  // return month where 1=Jan.
  int guess = dayOfYear()/30;
  return (dayOfYear() <= month_days[guess] ? guess : guess+1);
}

//-----------------------------------------------------------------------------
inline const int Gregorian::day() const
{
  const std::vector<int>& month_days =
    (isLeapYear(year()) ? c_leapdaycount : c_daycount);

  return dayOfYear() - month_days[month()-1]; 
}

//-----------------------------------------------------------------------------
// Do a Google on "Zeller's Congruence" if you want to figure out where this
// came from.  Or, try this one: http://www.merlyn.demon.co.uk/zeller-c.htm
//
inline const int Gregorian::dayOfWeek() const
{
  int m(month()), y(year());
  if (m < 3) { 
    m = m + 12; 
    y = y-1;
  }
  return (2 + day() + (13*m-2)/5 + y + y/4 - y/100 + y/400) % 7;
}


//-----------------------------------------------------------------------------
// Warning!  GetDayOfYear() is 1->365.  Days since epoch is zero-based.
inline const int Gregorian::dayOfYear() const
{ return EpochCounter::days() - countDays(year()) + 1; }

//-----------------------------------------------------------------------------
inline void Gregorian::set(const int y, const int m, const int d)
{
  if (m<1 || m>12 || y<0) throw DateValueOutOfRangeException();
  bool leap = isLeapYear(y);
  const std::vector<int>& last_day = leap ? c_leaplastday : c_lastday;
  if (d<1 || d>last_day[m]) throw DateValueOutOfRangeException();

  int days = countDays(y);  
  const std::vector<int>& month_days = leap ? c_leapdaycount : c_daycount;

  days += (month_days[(m-1)] + d) - 1;  // epoch is the zero'th day.
  EpochCounter::days(days);
}

//-----------------------------------------------------------------------------
inline const int Gregorian::hour() const
{ return ticks() / EpochCounter::TICKS_PER_SECOND / 60 / 60; }

//-----------------------------------------------------------------------------
inline const int Gregorian::minute() const
{ return ticks() / EpochCounter::TICKS_PER_SECOND / 60 % 60; }

//-----------------------------------------------------------------------------
inline const int Gregorian::second() const
{ return ticks() / EpochCounter::TICKS_PER_SECOND % 60; }

//-----------------------------------------------------------------------------
inline const int Gregorian::subSecond() const
{ return ticks() % EpochCounter::TICKS_PER_SECOND; }

//-----------------------------------------------------------------------------
inline void Gregorian::time(const int hour, const int min, const int sec)
{
  if (hour<0 || hour>23) throw DateValueOutOfRangeException();
  if (min<0 || min>59) throw DateValueOutOfRangeException();
  if (sec<0 || sec>60) throw DateValueOutOfRangeException();  // leap seconds?

  int ticks = (sec * EpochCounter::TICKS_PER_SECOND) 
            + (min * EpochCounter::TICKS_PER_MINUTE) 
            + (hour * EpochCounter::TICKS_PER_HOUR);
  EpochCounter::ticks(ticks);
}

//-----------------------------------------------------------------------------
inline void Gregorian::setMidnight()
{ ticks(0); }

//-----------------------------------------------------------------------------
inline const struct tm Gregorian::getTimeStruct() const
{
  struct tm timestruct;
  timestruct.tm_sec  = this->second();
  timestruct.tm_min  = this->minute();
  timestruct.tm_hour = this->hour();
  timestruct.tm_mday = this->day();
  timestruct.tm_wday = this->dayOfWeek();
  timestruct.tm_mon  = this->month() - 1;   // UNIX tm struct months 0-11.
  timestruct.tm_year = this->year() - 1900; // UNIX tm struct starts at 1900 AD.
  return timestruct;
}

} // namespace dragonfly

#endif //__GREGORIAN_H__
