
// Copyright 2006 Mike Desjardins.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "gregorian.h"

namespace dragonfly {
  
const int Gregorian::c_daycount_a[] = 
  {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365, 365};

const int Gregorian::c_leapdaycount_a[] = 
  {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366, 366};

const std::vector<int> 
       Gregorian::c_daycount(Gregorian::c_daycount_a
     , Gregorian::c_daycount_a + (sizeof(c_daycount_a)/sizeof(int)));

const std::vector<int> 
       Gregorian::c_leapdaycount(Gregorian::c_leapdaycount_a
     , Gregorian::c_leapdaycount_a + (sizeof(c_leapdaycount_a)/sizeof(int)));
  
const int Gregorian::c_lastday_a[12] = 
  {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

const int Gregorian::c_leaplastday_a[12] = 
  {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

const std::vector<int> Gregorian::c_lastday(Gregorian::c_lastday_a[0]
                                          , Gregorian::c_lastday_a[11]);

const std::vector<int> Gregorian::c_leaplastday(Gregorian::c_leaplastday_a[0]
                                              , Gregorian::c_leaplastday_a[11]);

} // namespace dragonfly

