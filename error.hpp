#ifndef ERROR_HPP_
#define ERROR_HPP_ 1

//#include "utils.hpp"
//#include "info.hpp"
#include "preserve.hpp"



namespace util {

/// Variadic template cerror() debugging helper
    template<typename stream_T, typename... Args_T>
    void
info2stream(stream_T& _stream, const Args_T&... _args)
{
    LOCAL_MODIFIED(INFO_STREAM_PTR);
    INFO_TO(_stream);
    info( _args...);
}

/// Variadic template cerror() debugging helper
    template<typename... Args_T>
    void
cerror(const Args_T&... _args)
{
    info2stream(std::cerr,_args...);
}


}// namespace util

//:CERROR
// enable/disable debug helper info()
#if 1
/// enabled: better then printf easy to use (with format specifier) printing for the sole purpose of debugging
# define CERROR(...) util::cerror(__VA_ARGS__)
#else
/// disabled: better then printf easy to use (with format specifier) printing for the sole purpose of debugging
# define CERROR(...)
#endif

#if DEBUG
/// DEBUG enabled: better then printf easy to use (with format specifier) printing for the sole purpose of debugging
# define DBG_CERROR(...) CERROR(__VA_ARGS__)
#else
/// DEBUG disabled: better then printf easy to use (with format specifier) printing for the sole purpose of debugging
# define DBG_CERROR(...)
#endif

#if ALLOC_DEBUG
/// ALLOC_DEBUG enabled: better then printf easy to use (with format specifier) printing for the sole purpose of debugging
# define ALDBG_CERROR(...) CERROR(__VA_ARGS__)
#else
/// ALLOC_DEBUG disabled: better then printf easy to use (with format specifier) printing for the sole purpose of debugging
# define ALDBG_CERROR(...)
#endif

#define INFO2STREAM util::info2stream


#endif // ERROR_HPP_