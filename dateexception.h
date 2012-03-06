#ifndef __DATEEXCEPTION_H__
#define __DATEEXCEPTION_H__

// Copyright 2006 Mike Desjardins.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <stdexcept>

namespace dragonfly {
  class DateTimeException : public std::exception {};
  class DateValueOutOfRangeException : public DateTimeException {};
  class DateParsingException: public DateTimeException {};
  class DateBadFormatElement: public DateTimeException {};
}

#endif /*__DATEEXCEPTION_H__*/
