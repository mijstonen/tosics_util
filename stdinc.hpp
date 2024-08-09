/// -*- C++ -*- file: stdinc.hpp  ///

#ifndef STDINC_H_
#define STDINC_H_ 1
#define _GLIBCXX_USE_NANOSLEEP

// C basic (with *.h)
#include <stdc-predef.h>
#include <assert.h>
#include <ext/atomicity.h>
#include <ext/aligned_buffer.h>
#include <linux/version.h>
#include <pthread.h>
#include <endian.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <rpc/netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <dirent.h>
#include <features.h>
#include <execinfo.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/eventfd.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/epoll.h>
#include <sys/uio.h>
#include <sys/un.h>
#include <sys/cdefs.h>
#include <sys/timerfd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>

// C enclosed (with cstd*)
#include <cerrno>
#include <cassert>
#include <system_error>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <csignal>
#include <ctime>
#include <cstdlib>
#include <climits>
#include <cctype>
#include <cstdio>
#include <cwctype>
#include <cwchar>
#include <cmath>
#include <cstdarg>
#include <csetjmp>

// C++ basic
#include <limits>
#include <type_traits>
#include <typeinfo>
#include <initializer_list>
#include <locale>
#include <string>
#include <string_view>
#include <typeinfo>
#include <memory>
#include <new>
#include <typeindex>
#include <stdexcept>
#include <ranges>
#include <concepts>

// C++ containers
#include <algorithm>
#include <iterator>
#include <vector>
#include <deque>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <tuple>
#include <array>

// C++ streams
#include <streambuf>
#include <iosfwd>
#include <istream>
#include <ostream>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>

// C++ stl
#include <memory>
#include <bitset>
#include <iterator>
#include <array>
#include <stack>
#include <queue>
#include <deque>
#include <list>
#include <forward_list>

//C++11 threading
#include <utility>
#include <future>
#include <thread>
#include <condition_variable>
#include <stop_token>
#include <atomic>
#include <any>

// C++ math
#include <complex>
#include <valarray>

// C++ZZ
#include <regex>
#include <chrono>
#include <filesystem>
#include <optional>
#include <source_location>
#include <coroutine>
#include <numbers>
#include <stacktrace>

// boost adhock desires
#include <boost/current_function.hpp>
#include <boost/core/demangle.hpp>

#if 0 // get missing pieces from boost
#include <boost/circular_buffer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/any.hpp>
#endif

#if 0 // also include all the goodies of chai
#include <chaiscript/chaiscript.hpp>
#endif

#if 1 // bare OpenMP ( requires -fopenmp )
#include <omp.h>
#endif

#if 1 // for cpx, reduces guard checking and gave scripts cpx alike capabilities
#include <pstream.h>
#include <TinySHA1.hpp>
#endif
/*
________________________________________________________________________________________________________________________

*/
// documentation on C(++)(0x) see
//     http://www.cplusplus.com/
//     http://en.cppreference.com
/**
 This include file is designed to be precompiled with
    g++ -Wall -std=<latest> "$COMMON_OPTIONS" stdinc.h -o stdinc.h.gch

 and used as

    g++ -Wall -std=<latest> -include stdinc.hpp "$COMMON_OPTIONS"<sources> ....

 for <latest> see man gcc search for -std options, with newer gcc versions, these change often

     examples
     -std=c++14 .... -std=gnu++23

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
