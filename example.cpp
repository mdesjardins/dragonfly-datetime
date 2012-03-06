// Copyright 2006 Mike Desjardins.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "datetime.h"
#include "dateformatter.h"
#include "duration.h"

#include <iostream>

using namespace dragonfly;

// function:  test_range
// params:    month - a month, 1 through 12 inclusive.
//            totaldays - the number of days in that month.
//            year - a year
// called by: main()
// purpose:   iterate through each day of the month for the
//            supplied year.  Set the date to that particular
//            day, then test to make sure the same date is
//            returned.  This function effectively verifies  
//            that the algorithm for setting and determining 
//            the date based on the number-of-days since epoch
//            is functioning correctly.
//
void test_range(int month, int totaldays, int year)
{
  DateTime mydate;
  for (int day=1; day<=totaldays; ++day)
  {
     mydate.set(year, month, day);
     if (mydate.year() != year)
      std::cerr << "Bad Year: (" << mydate.year() << ")"
                << month << "/" << day << "/" << year 
                << " [" << mydate.days() << "]"
                << " [" << mydate.dayOfYear() << "]"
                << std::endl;   
                
     if (mydate.month() != month)
      std::cerr << "Bad Month: (" << mydate.month() << ")"
                << month << "/" << day << "/" << year 
                << " [" << mydate.days() << "]"
                << " [" << mydate.dayOfYear() << "]"
                << std::endl;   
                
     if (mydate.day() != day)
      std::cerr << "Bad Day: (" << mydate.day() << ")"
                << month << "/" << day << "/" << year 
                << " [" << mydate.days() << "]"
                << " [" << mydate.dayOfYear() << "]"
                << std::endl;   
  }
}

// function:  main
// params:    argv, argc
// called by: n/a
// purpose:   runs through some tests and examples of using the date/time
//            classes.
//
int main(int argc, char* argv[])
{
  DateTime testdate;
  
  testdate.set(1900,1,1);
  std::cerr << "1/1/1900 - " << testdate.days() << std::endl;
  
  testdate.set(2000,1,1);
  std::cerr << "1/1/2000 - " << testdate.days() << std::endl;
  
  testdate.set(2100,1,1);
  std::cerr << "1/1/2100 - " << testdate.days() << std::endl;
  
  for (int year=1600; year<=2200; ++year)
  {    
    if (year%100 == 0)
      std::cerr << "Testing years: " << year << " thru " << year+99 << std::endl;

    test_range(1,31,year);
    if (year>0 && (!(year%4) && ( (year%100) || !(year%400) )))
      test_range(2,29,year);
    else
      test_range(2,28,year);
    test_range(3,31,year);
    test_range(4,30,year);
    test_range(5,31,year);
    test_range(6,30,year);
    test_range(7,31,year);            
    test_range(8,31,year);            
    test_range(9,30,year);            
    test_range(10,31,year);            
    test_range(11,30,year);            
    test_range(12,31,year);            
  }
  
  // Test time functions.  Similar to the test_range function.  Iterates
  // through each hour/minute/second of a single day to make sure what we
  // set matches what we return.  Verifies that the calculation to/from ticks
  // is working correctly.
  //
  std::cerr << "Testing Time..." << std::endl;
  DateTime testtime;
  for (int hour=0; hour<24; ++hour) {
    std::cerr << " Testing Hour " << hour << std::endl;
    for (int minute=0; minute<60; ++minute) {
      for (int second=0; second<60; ++second) {
             testtime.time(hour,minute,second);
             if (testtime.hour() != hour)
                std::cerr << "Bad Hour " << hour << ":" << minute << ":" 
                          << second << " - " << hour << "!=" << testtime.hour()
                          << std::endl;
                          
             if (testtime.minute() != minute)
                std::cerr << "Bad Hour " << hour << ":" << minute << ":" 
                          << second << " - " << hour << "!=" << testtime.hour()
                          << std::endl;
             
             if (testtime.second() != second)
                std::cerr << "Bad Hour " << hour << ":" << minute << ":" 
                          << second << " - " << hour << "!=" << testtime.hour()
                          << std::endl;
      }
    }
  }
  std::cerr << "Done." << std::endl;
  
  // Examples of how to use the day-of-week functions.
  //
  std::cerr << "Testing Day-of-week..." << std::endl;
  DateTime dow(2005,2,27);
  std::cerr << " 2/27/2005 falls on day " << dow.dayOfWeek() 
            << " of the week." << std::endl;
  
  dow.set(2005,3,1);
  std::cerr << " 3/1/2005 falls on day " << dow.dayOfWeek() 
            << " of the week." << std::endl;
  std::cerr << "Done." << std::endl;

  // Examples of how to use durations.
  //
  std::cerr << "Testing duration..." << std::endl;
  DateTime begin(2005,04,07), end(2005,04,17);
  Duration span = (end - begin);
  std::cerr << " 4/7/2005 -> 4/17/2005: " << span.days() << " days." << std::endl;
  std::cerr << " 4/7/2005 -> 4/17/2005: " << span.hours() << " hours." << std::endl;
  std::cerr << "Done." << std::endl;
  
  // Examples of how to use the formatter/parser.
  //
  std::cerr << "Testing formatter..." << std::endl;
  DateFormatter formatter_1("%m-%d-%Y"), 
                formatter_2("%A, %B %e, %Y %I:%M%p"), 
                formatter_3("%Y%m%d%H%M%S"),
                formatter_4(""),
                formatter_5("%x");
  DateTime test_format(2002,06,10);
  std::cerr << formatter_1.format(test_format) << std::endl
            << formatter_2.format(test_format) << std::endl
            << formatter_3.format(test_format) << std::endl
            << formatter_4.format(test_format) << std::endl
            << formatter_5.format(test_format) << std::endl;
  
	std::string test_parse_1("20050411");
	DateFormatter formatter_6("%Y%m%d");
	DateTime parsed = formatter_6.parse(test_parse_1);
	std::cerr << formatter_1.format(parsed) << std::endl;

	std::string test_parse_2("05/04/1974");
	DateFormatter formatter_7("%m/%d/%Y");
	parsed = formatter_7.parse(test_parse_2);
	std::cerr << formatter_1.format(parsed) << std::endl;

  std::string test_parse_3("June 15 1975");
  DateFormatter formatter_8("%B %d %Y");
  parsed = formatter_8.parse(test_parse_3);
  std::cerr << formatter_1.format(parsed) << std::endl;

  std::string test_parse_4("Monday, Jun 10, 2002");
  DateFormatter formatter_9("%A, %b %d, %Y");
  parsed = formatter_9.parse(test_parse_4);
  std::cerr << formatter_1.format(parsed) << std::endl;

  // one with the time, too...
  std::string test_parse_5("April-17-06 8:49PM");
  DateFormatter formatter_10("%B-%d-%y %l:%M%p");
  parsed = formatter_10.parse(test_parse_5);
  std::cerr << formatter_3.format(parsed) << std::endl;

  return 0; 
}
