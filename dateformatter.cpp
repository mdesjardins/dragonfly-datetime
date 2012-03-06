// Copyright 2006 Mike Desjardins.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "dateformatter.h"
#include "dateexception.h"

#include <iostream>
#include <sstream>
#include <functional>
#include <algorithm>

namespace dragonfly {

// Format tags recognized by the DateFormatter class:
//
//       %%     a literal %
//       %a     locale's abbreviated weekday name (Sun..Sat)
//       %A     locale's full weekday name, variable length (Sunday..Saturday)
//       %b     locale's abbreviated month name (Jan..Dec)
//       %B     locale's full month name, variable length (January..December)
//       %C     century (year divided by  100  and  truncated  to  an  integer)
//              [00-99]
//       %d     day of month (01..31)
//       %D     date (mm/dd/yy)
//       %e     day of month, blank padded ( 1..31)
//       %h     same as %b
//       %H     hour (00..23)
//       %I     hour (01..12)
//       %j     day of year (001..366)
//       %k     hour ( 0..23)
//       %l     hour ( 1..12)
//       %m     month (01..12)
//       %M     minute (00..59)
//       %p     locale's upper case AM or PM indicator (blank in many locales)
//       %P     locale's lower case am or pm indicator (blank in many locales)
//       %S     second (00..59)
//       %w     day of week (0..6);  0 represents Sunday
//       %W     week number of year with Monday as first day of week (00..53)
//       %y     last two digits of year (00..99)
//       %Y     year (1970...)

// function: format
// params:   DateTime object
// returns:  A string, formatted as specified by format_ instance variable.
// purpose:  Pass in a DateTime, returns a text string representation of that
//           calendar date and/or time.
//
const std::string DateFormatter::format(const DateTime& date)
{ 
  std::ostringstream os;
  
  // Get a copy of the date held by the formatter converted to a POSIX thing...
  std::tm ts = date.getTimeStruct();

  // Get the facet that is currently installed on this output stream.
  std::time_put<char> const& facet = 
    std::use_facet< std::time_put<char> >( os.getloc() );

  // Do the put...
  const char* pat_beg = format_.c_str();
  const char* pat_end = pat_beg + format_.size();
  if (facet.put(os, os, os.fill(), &ts, pat_beg, pat_end).failed())
    os.setstate(os.badbit);

  return os.str(); 
}

// function: parse
// params:   text: string containing a text representation of a date, to be
//                 parsed.
//           loc: an optional locale object.  If not provided, the method
//                will retrieve the default locale.
// returns:  A DateTime object parsed from the string supplied to the method.
// purpose:  Reads a text representaion of a date in the supplied string, and
//           returns a DateTime object set to the date and time described by
//           that string.
//
const DateTime DateFormatter::parse(const std::string& text, 
                                    const std::locale& loc /* = std::locale() */ )
{ 
#if __USE_STRPTIME
	//
	// Some C libraries come with a function called strptime, which does all
	// of the parsing for us.  If this is one of the lucky environments that has
	// sports such an animal, then feel free to use it.  This hasn't been tested.
  // Good luck.  Let me know if it works.
	//
  struct tm ts;
  char* end = std::strptime(s.c_str(), format_.c_str(), &ts);
  if (end == 0)
    throw DateParsingException();
  return DateTime(ts);
#else
 	TimeStruct ts; 	 
	unsigned int ti = 0;  // text (input) string index.  

  // TODO - I probably oughtta use iterators instead of character indexes.
  for (unsigned int fi = 0; fi < format_.size(); ++fi) {
    //
    // The format string will consist of one of the following:
    //    1.) Whitespace, in which case we keep reading the
    //        input string until we no longer find any whitespace.
    //    2.) An ordinary character, in which case we compare
    //        the character to the corresponding string value 
    //        for a match.
    //    3.) A percent sign, followed by a modifier.  This 
    //        indicates to the algorithm that we are to interpret
    //        the next portion of the string as a part of a date.
    //    4.) A percent sign followed by another percent sign.
    //        This is interpreted as a literal %
    //
    char fmt_el = format_[fi];
    switch (fmt_el) {
      case ' ':
      case '\t':
      case '\n':
      case '\r':
        // whitespace.  increment the text pointer.  if the next character is
        // still whitespace, back up the format pointer and keep trying.  otherwise,
        // move onto the next formatting element.
        //
        ++ti;
        if (text[ti] == ' ' || text[ti] =='\t' || text[ti] == '\n' || text[ti] == '\r')
          --fi;
        break;
      case '%':
        ++fi;
        if (format_[fi] != '%') {
	        ti += parse_datepart(format_[fi],text.substr(ti,text.size()),loc,ts);
          break;
        }         	
        else
					// treat as ordinary character, fall through...
					;
      default:
        // ordinary character
        if (text[ti] != format_[fi])
        	throw DateBadFormatElement();
        else {
        	++ti;
        }
    }
  }

  // get_date from the stream and output tm contents.
  //tg.get_date(begin, end, is, state, &s);
  return DateTime(ts.tm_year, ts.tm_mon, ts.tm_mday, ts.tm_hour, ts.tm_min, ts.tm_sec);
#endif  // __USE_STRPTIME
}

} // namespace dragonfly
