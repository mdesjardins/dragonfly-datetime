#ifndef __DATEFORMATTER_H__
#define __DATEFORMATTER_H__

// Copyright 2006 Mike Desjardins.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "datetime.h"
#include "dateexception.h"
#include <string>
#include <locale>
#include <sstream>
#include <iostream>
#include <map>

namespace dragonfly {

// class:    DateFormatter
// purpose:  This class is responsible for
//             - Formatting a DateTime object into a user-defined human-
//               readable string, or
//             - Parsing a user-defined formatted string date into a
//               DateTime object.
//           The two methods which accomplish these tasks are format and parse.
//           Both format and parse use the following format tags to indicate
//           their respective fields withing the date/time strings (these
//           roughly correspond to those defined in std::ctime):
//
//       %%     a literal %
//       %a     locale's abbreviated weekday name (Sun..Sat)
//       %A     locale's full weekday name, variable length (Sunday..Saturday)
//       %b     locale's abbreviated month name (Jan..Dec)
//       %B     locale's full month name, variable length (January..December)
//       %C     century (year divided by  100  and  truncated  to  an  integer)
//              [00-99]
//       %d     day of month (01..31)
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
//       %y     last two digits of year (00..99)
//       %Y     year (1970...)
//
class DateFormatter {
	public:
    DateFormatter(const std::string& format) : format_(format) {}
  public:
    const std::string format(const DateTime& date);
  public:
    const DateTime parse(const std::string& text, 
                         const std::locale& loc = std::locale());
    unsigned int parse_datepart(const char fmt, const std::string& text,
                                const std::locale& loc, std::tm& ts);
    unsigned int parse_ampm(const std::string& text, const std::locale& loc, 
                            std::tm& ts);
  private:
    static const int WRAP_ = 50;  // where we split the century on two-digit years.
  private:
    std::string format_;
};

// class:   TimeStruct
// purpose: Wraps the POSIX time struct to automate the initialization of it.
//
class TimeStruct : public std::tm {
  public:
    TimeStruct() {
      std::memset(this, 0, sizeof(*this));
    }
};

// class:   LocaleParser<char,int>
// purpose: I ripped this concept off from Boost's date library written by Jeff 
//          Garland (http://www.boost.org).  We need a place to store the 
//          locale-specific month names, mostly because I couldn't get 
//          time_get's get_monthname to work.  Boost's date_time has a 
//          gather_month_strings method that does a put on each month, and 
//          stuffs the results into a vector to be used by parsing functions 
//          later.  I figured I'd do something similar rather than continue to
//          fight w/ GNU's get_monthname function (although, frankly, I'd rather 
//          get that to work instead of crutching by on this thing).
//
//          I did a map instead of a vector.  Don't know if that was wise or 
//          stupid.
//
//          I would've made everything in this class static, but the GNU 
//          compiler didn't seem to like that with the template.
//
template <char format, int max>
class LocaleParser {
  public:
    LocaleParser() : delim(" ,/-.!@#$%^&*()[]{};:<>?|\\") {}
  private:
    std::map<std::string,int> names_;
    const std::string delim;
    
  private:
    void load() {
      TimeStruct ts;
      std::ostringstream os;
  
      // Get the facet that is currently installed on this output stream.
      std::time_put<char> const& facet = 
        std::use_facet< std::time_put<char> >( os.getloc() );

      // Do the put for each month/weekday, grab the string in the output 
      // stream, and stick it in the map...
      const char pat[2] = { '%', format };
      for (int x=0; x<max; ++x) {
        ts.tm_mon = x;
        ts.tm_wday = x;
        facet.put(os, os, os.fill(), &ts, pat, pat + sizeof(pat));
        names_[os.str()] = x + 1;
        os.str("");
      }

//      std::map<std::string,int>::iterator i = names_.begin();
//      for ( ; i != names_.end(); ++i ) {
//        std::cerr << ">" << i->first << "< ,  >" << i->second << "<" << std::endl;
//      }
    }
  public:
    int get_val(const std::string& name, int& output) {
      if (names_.empty()) {
        load();
      }

      int pos = name.find_first_of(delim);
      std::string check = name.substr(0,pos);
      std::map<std::string,int>::const_iterator i;
      if ((i = names_.find(check)) != names_.end())  {
        output = i->second;
        return pos;
      }
      else {
        throw DateParsingException();
      }
    }
};

// Specialize one LocaleParser for each type of thing we're parsing...
typedef LocaleParser<'b',12> AbbrevMonth;
typedef LocaleParser<'B',12> FullMonth;
typedef LocaleParser<'a',7> AbbrevWeekday;
typedef LocaleParser<'A',7> FullWeekday;

// GNU compiler doesn't let me next this inside the parse_datepart method.
template<unsigned int digits, unsigned int min, unsigned int max>
class Convert {
	public:
		// wanted this thing to work even when non-numerics appeared.  
		static unsigned int get(const std::string& s, int& i) {
			i = 0;
      int j = 0;
			for (unsigned int pos=0; pos<digits; ++pos) {
				if (isdigit(s[pos])) {
				  i *= 10;
					i += s[pos] - '0';
				  ++j;
        }
			}
      return j;
		}
};

// function:  DateFormatter:parse_datepart
// params:    fmt: a single format character (see class def. for valid contents)
//            text: string to parse for this portion of the date
//            loc: locale object
//            ts: POSIX time struct for storage of results.
// called by: DateFormatter::parse
// returns:   Number of characters read and parsed
// purpose:   The parse method reads through the format string for characters
//            following a % character which are not themselves % characters.
//            When it encounters one, it calls this method with the remaining
//            unparsed date string along with the character describing the
//            expected date field.  The method reads the string for digits
//            or text corresponding to the format character and populates the
//            timestruct passed to it.
//
inline 
unsigned int DateFormatter::parse_datepart(const char fmt, 
                                           const std::string& text,
                                           const std::locale& loc, std::tm& ts) 
{
  // specialize a converter for each numeric field we parse.
	typedef Convert<2,1,31> DayOfMonth;
	typedef Convert<3,1,366> DayOfYear;
	typedef Convert<2,1,12> MonthOfYear;
	typedef Convert<4,0,9999> Year;
  typedef Convert<2,0,99> LittleYear;
	typedef Convert<2,0,99> Century;
	typedef Convert<2,0,23> Hour;
	typedef Convert<2,1,12> SemiHour;
	typedef Convert<2,0,59> Minute;
	typedef Convert<2,0,59> Second;
	
  // Local versions of the locale-based string parsers.
  static AbbrevMonth abbrev_month;
  static FullMonth full_month;
  static AbbrevWeekday abbrev_weekday;
  static FullWeekday full_weekday;

  switch (fmt) {
    case 'a':
      // weekday name (a=abbrev, A=full)
      return abbrev_weekday.get_val(text, ts.tm_wday);
      break;
    case 'A':
      // weekday name (a=abbrev, A=full)
      return full_weekday.get_val(text, ts.tm_wday);
      break;
    case 'b':
    case 'h':
      // month name (b,h=abbrev, B=full)
      return abbrev_month.get_val(text, ts.tm_mon);
      break;
    case 'B':
      // month name (b,h=abbrev, B=full)
      return full_month.get_val(text, ts.tm_mon);
      break;
    case 'C':
      // century (year divided by  100  and  truncated  to  an  integer)
      return Century::get(text, ts.tm_year);
      break;     
    case 'd':
    case 'e':
      // day of month, zero padded.
      return DayOfMonth::get(text, ts.tm_mday);
      break;
    case 'H':
      // hour (00..23)
    case 'k':
      // hour, blank padded, ( 0..23)
      return Hour::get(text, ts.tm_hour);
      break;
    case 'l':
    case 'I':
      // I: hour (01..12)
      // l: hour, blank padded, ( 1..12)
      return SemiHour::get(text, ts.tm_hour);
      break;
    case 'j':
      // day of year (001..366)  <<TODO>>
      //ts.day = DayOfYear::get(text[offset]);
      //break;
    case 'm':
      // month, (01..12)
      return MonthOfYear::get(text, ts.tm_mon);
      break;
    case 'M':
      // minute, (00..59)
      return Minute::get(text, ts.tm_min);
      break;
    case 'p':
    case 'P':
      // p: locale's upper case AM or PM indicator (may be blank)
      // P: locale's lower case am or pm indicator (may be blank)
      return parse_ampm(text, loc, ts);
      break;
    case 'S':
      // seconds (00..59)
      return Second::get(text, ts.tm_sec); 
      break;
    case 'Y':
      // year (e.g., 1970..)
      return Year::get(text,ts.tm_year);
      break;
    case 'y':
      { // scope
        // last two digits of the year (e.g., 70...).
        int year = 0, retval = LittleYear::get(text, year);
        if (year < WRAP_)
          ts.tm_year = year + 2000;
        else
          ts.tm_year = year + 1900;
        return retval;
        break;
      }
    default:
      throw DateBadFormatElement();
  } 
  return 0;
}

// function:  parse_ampm
// params:    text: remaining unparsed date string to be parsed
//            loc: locale object on this stream.
//            ts: POSIX time struct.
// returns:   integer indicating the number of characters read and parsed.
// called by: parse_datepart
// purpose:   reads the string passed into it searching for locale-specific
//            am/pm indicator.
//
inline
unsigned int DateFormatter::parse_ampm(const std::string& text, 
                                       const std::locale& loc, std::tm& ts)
{
  static std::string am_upper;
  static std::string am_lower;
  static std::string pm_upper;
  static std::string pm_lower;
  if (am_upper.empty() || pm_upper.empty() || 
      am_lower.empty() || pm_lower.empty()) {
    // still need to determine this locale's AM/PM string.
    TimeStruct temp;
    std::ostringstream os;
  
    // Get the facet that is currently installed on this output stream.
    std::time_put<char> const& facet = 
      std::use_facet< std::time_put<char> >( os.getloc() );

    { // upper
      const char pat[2] = { '%', 'p' };
      facet.put(os, os, os.fill(), &temp, pat, pat + sizeof(pat));
      am_upper = os.str();
      os.str("");
      temp.tm_hour = 13;
      facet.put(os, os, os.fill(), &temp, pat, pat + sizeof(pat));
      pm_upper = os.str();
      os.str("");
    }
    
    { // lower
      const char pat[2] = { '%', 'P' };
      facet.put(os, os, os.fill(), &temp, pat, pat + sizeof(pat));
      temp.tm_hour = 0;
      am_lower = os.str();
      os.str("");
      temp.tm_hour = 13;
      facet.put(os, os, os.fill(), &temp, pat, pat + sizeof(pat));
      pm_lower = os.str();
      os.str("");      
    } 
  }

  if (text.find_first_of(pm_upper) != std::string::npos ||
      text.find_first_of(pm_lower)) {
    // we found a PM indicator.  Adjust the hour accordingly if need be.
    if (ts.tm_hour < 13)
      ts.tm_hour += 12;

    return pm_upper.size();
  }
  
  // For AM, we don't need to do anything, but we still throw if we *don't*
  // see it.
  //
  if (text.find_first_of(am_upper) == std::string::npos &&
      text.find_first_of(am_lower) == std::string::npos) {
    throw DateParsingException();
  }

  return am_upper.size();
}

} // namespace dragonfly

#endif /* __DATEFORMATTER_H__ */
