I wrote this class for manipulating dates in C++. It’s inspired by the Java’s Date and 
SimpleDateFormat classes. Boost has their own date class. Boost’s has a heck of a lot
more features, but it’s also comprised of a lot more files and (IMHO) more complicated. 
Part of it reeks of what I don’t like about C++ (who the heck wants date iterators, 
anyway? Why represent a range of time with a pair of dates? Ech.) Anyway, this was a 
labor of love. Use it however you see fit… it’s licensed under the super-permissive 
Boost license.

# Features of my date class include:

* Locale-sensitive, flexible formatting of dates and times

* Works well beyond the UNIX 2038 barrier without requiring 64 bit longs

* Intuitive interaction between durations and calendar date/time

* Capable of working w/ extremely short intervals of time

# Stuff my date class can’t do:

* It doesn’t deal with that weird time in October of 1582, when the calendar “lost” ten 
days by decree of Pope Gregory XIII. So any date difference operations that you do 
around those days won’t take the missing days into account. (Learn more than you
ever wanted to know about this stuff
[here](http://en.wikipedia.org/wiki/Gregorian_Calendar).)

* It doesn’t do leap seconds.

* It thinks that Jan 1, 0 A.D. is a valid date. In fact, this is the “epoch” begining 
of time to this class. Purists will note that the year 0 A.D. not valid.

* It might not do dates before the theoretical 0 A.D. It might work, but I haven’t 
really tested it. Caveat Emptor.

I’ve only compiled it with GCC so far. I had bigger hopes for this thing (doxygen 
documentation, compiling on a bunch of compilers, more unit tests), but I ran out 
of time and ambition, and I don’t do a lot with C++ lately. So here it is. If anyone 
shows interest in it, fork it and do whatever you want! Figured I might as well 
share it if anyone finds it useful.

