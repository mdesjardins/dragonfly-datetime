#
# Copyright 2006 Mike Desjardins.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
# 
LIB_SONAME=libdflydate.so.1
LIBNAME=libdflydate.so
CC=g++
AR=ar
CCOPTS=-g -c -Wall -Wl,-export-dynamic -mno-cygwin
PIC=-fPIC
STATIC=-static
LIBOPTS=-shared -Wl,-soname,$(LIB_SONAME) -mno-cygwin

LIBSRCS = dateformatter.cpp epochcounter.cpp gregorian.cpp
LIBOBJS = $(LIBSRCS:%.cpp=%.o)

example.o: 
	$(CC) -c example.cpp

.cpp.o:
	$(CC) $(CCOPTS) $(PIC) $^

dynamiclib: $(LIBOBJS)
	$(CC) $(LIBOPTS) -o $(LIBNAME) dateformatter.o epochcounter.o gregorian.o -lc

staticlib: $(LIBOBJS)
	$(AR) rcs libdflydate.a $(LIBOBJS)

example: example.o
#	$(CC) $(STATIC) example.cpp -o example -L. -lm -ldflydate 
	$(CC) example.cpp -o example -L. -ldflydate
	