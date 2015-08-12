/// -*- C++ -*- file: stdinc.h ///

#ifndef STDINC_H_
#define STDINC_H_ 1
#define _GLIBCXX_USE_NANOSLEEP

#define DEBUG_TRACE 1
#include <trace.h>

#if 1 // C basic (with *.h)
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#endif

#if 1 // C enclosed (with cstd*)
#include <cstdint>
#include <cstdlib>
#include <cstddef>
#include <cstdarg>
#endif

#if 1 // C enclosed (with c*)
#include <cassert>
#include <cctype>
#include <cstring>
#include <cstdio>
#include <climits>
#include <cerrno>
#include <csetjmp>
#endif

#if 1 // C++ basic
#include <limits>
#include <typeinfo>
#include <typeindex>
#include <stdexcept>
#include <new>
#endif

#if 1 // C++ streams
#include <iostream>
#include <iomanip>
#include <fstream>
#include <streambuf>
#include <sstream>
#endif

#if 1 // C++ stl minimum
#include <algorithm>
#include <vector>
#include <utility>
#endif

#if 1 // C++ stl
#include <memory>
#include <bitset>
#include <iterator>
#include <map>
#include <stack>
#include <queue>
#include <deque>
#include <list>
#endif

#if 0 // C++ math
#include <complex>
#endif

// C++0x
#include <type_traits>
#if 0
#include <tuple>
#include <regex>
#include <thread>
#include <chrono>
#include <functional>
#endif



/*
________________________________________________________________________________________________________________________

*/
// documentation on C(++)(0x) see
//     http://www.cplusplus.com/
//     http://en.cppreference.com
/**
 This include file is designed to be precompiled with
    g++ -Wall -std=c++0x "$COMMON_OPTIONS" stdinc.h -o stdinc.h.gch

 and used as

    g++ -Wall -std=c++0x -include stdinc.h "$COMMON_OPTIONS"<sources> ....

 where the <sources> not need to include  stdinc.h and none of its enlisted files (and possibly more)
 This saves compilation time and development time for most applications, because time required to
 figure out in which include file is which function (or which include file is missing) is greatly
 reduced

 note: "$COMMON_OPTIONS" represents options for compilation that needs to be the same for precompiling
       the headers and the <sources>. It mostly is specific per project. So you might consider separate
       precompiled headers per project

       see precompile.mk (TODO: create standard makefile template file to
       create a precompiled headerfile for a project )

 **/
/*
________________________________________________________________________________________________________________________

*/
#endif // STDINC_H_
