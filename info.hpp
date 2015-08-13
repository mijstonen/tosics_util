#ifndef INFO_HPP_
#  define INFO_HPP_ 1

#  include "utils.hpp"

#  define INFO_TO(TargetOstream_) util::Direct_info_to( & TargetOstream_ )
#  define ENDL util::NextLine_tag()


namespace util {

// Explicitly defining how objects are separated can be used to customize
// formatting and debugging the 'info' module.
constexpr char const* ObjectSeparation=" ";

// To count objects that are printed on the same line. By function encapsulation
// of the object we can have the counter in a header-only implementation. Generally
// global (static) variables are bad. Here it is somewhat improved by use of
// thread local storage. Also in forked processes and mpi based processes independed
// (threat) processes do not effect each other. It would be better to store
// context in a separate object and operate from there. That could be one of the
// future directions of info.
    inline short&
ObjOnLineCntRef()
{
    static thread_local short ObjOnLineCnt=0;
    return ObjOnLineCnt;
}

    inline std::ostream*&
OstreamPtrRef()
{
    static thread_local std::ostream*  osmptr= &std::cout;
    static thread_local std::ostream*& OstreamPtr= osmptr;
    return OstreamPtr;
}

    inline void
Direct_info_to( std::ostream* target_)
{
    OstreamPtrRef()= target_;
}

class NextLine_tag
{
  // This type is to let specilized template function do_stream( NextLine_tag)
  // issue a newline. The postfix '_tag' indicates that this class is used only
  // as a type selection mechanism by the compiler.
};

//______________________________________________________________________________

    template  <typename Object_T>
    inline void
now_stream_object( Object_T const& _object)
{
    ( *(OstreamPtrRef()) )<< _object;
}


// specialize for floating point types because they need to presented with the correct precision
    template  <typename Object_T>
    inline void
now_stream_object_fp( std::ostream* os_ , Object_T const& _object)
{
    (*os_) <<std::setprecision( std::numeric_limits<Object_T>::max_digits10) << "\'"<< _object << "\'";
}

    template<>
    inline void
now_stream_object<float>( float const& _object)
{
  now_stream_object_fp( OstreamPtrRef(), _object);
}
    template<>
    inline void
now_stream_object<double>( double const& _object)
{
  now_stream_object_fp( OstreamPtrRef(), _object);
}
    template<>
    inline void
now_stream_object<long double>( long double  const& _object)
{
  now_stream_object_fp( OstreamPtrRef(), _object);
}

//______________________________________________________________________________

    template<typename Value_T>
    inline std::string
single_quote( Value_T const& _value)
{
    std::stringstream ss;
    ss << "\'"<< _value<< "\'";
    return std::move( ss.str());
}
    template<>
    inline std::string
single_quote( float const& _value)
{
    std::stringstream ss;
    now_stream_object_fp( &ss, _value);
    return std::move( ss.str());
}
    template<>
    inline std::string
single_quote( double const& _value)
{
    std::stringstream ss;
    now_stream_object_fp( &ss, _value);
    return std::move( ss.str());
}
    template<>
    inline std::string
single_quote( long double const& _value)
{
    std::stringstream ss;
    now_stream_object_fp( &ss, _value);
    return std::move( ss.str());
}
    template<typename Value_T>
    inline std::string
single_quote( Value_T const& _value, bool hex)
{
    std::stringstream ss;

    if ( hex ) {
        ss << "\'0x"<< std::hex<< _value<< std::dec<< "\'";
    }
    else {
        ss << "\'"<< _value<< "\'";
    }
    return std::move( ss.str());
}
    template<>
    inline std::string
single_quote( bool const& _value)
{
    std::stringstream ss;
   ss << '\'' << (_value?"true":"false") << "\'";
    return std::move( ss.str());
}
//______________________________________________________________________________

// Only the do_stream() template function accesses the stream object (in this
// case std::cout). This has the advantage that the device may change to
// std::cerr, std::clog, a file e.g. without need to modify the actual use of
// the INFO macro's.
    template <typename Object_T>
    inline void
do_stream( Object_T const& _object)
{
    if ( ObjOnLineCntRef() >= 1 ) {
        ( *(OstreamPtrRef()) )<< ObjectSeparation;
    }
    ++(ObjOnLineCntRef());
    now_stream_object<Object_T>( _object);
}
    template <>
    inline
void do_stream( NextLine_tag const& /*_unused*/)
{
    ObjOnLineCntRef()=0;
    ( *(OstreamPtrRef()) )<< std::endl;
}
    inline void
info()
{
    do_stream( ENDL);
}
    template<typename Last_T>
    inline void
info( Last_T const& _last)
{
    do_stream( _last);
    info();
}
    template<typename First_T, typename... Remaining_T>
    inline void
info( First_T const& _first, Remaining_T const&... _remaining)
{
    do_stream( _first);
    info( _remaining...);
}


}// namespace util


//:INFO
/**
 @brief enabled: Lets you quicky and systematically print variables. Better for debugging then standard printing.
 @param ... expects any amount of objects that can be streamed to std::ostream.
 @sa VARVAL() PTRVAL() VARVALHEX() VARCHRNUM() VARCHRHEXNUM()

 \code INFO( a, b, c) \endcode
 does the same as
 \code std::cout << a << ' ' << b << ' ' << c << ' ' << std::endl \endcode
 but with less typing. It is intended to be used with macro's VARVAL,PTRVAL...VARCHRHEXNUM as a low level and low entry
 debugging aid. Those macro's make labels from the given variable and display the value (as of cout<< object standard
 or self defined operator << ) in single quotes.

 @b examples
 \code
 float my_value = 26.0; int div_num=5;

 // simple INFO example
 // -------------------
 INFO("my value text is",my_value","devided by",divnum,"is",my_value/div_num);

 // output "my value text is 26.000000 divided by 5 is 5.200000\n";

 // INFO with VARVAL example
 // ------------------------
 INFO(VARVAL(my_value),"divided by",VARVAL(div_num),"is",VARVAL(my_value/div_num));
 // produces
 std::cout<<"my_value"<<"="<<' '<<''''<<my_value<<''''<<' '
          <<"divided by"<<' '<<"div_num<<"="<<' '<<''''<<div_num<<''''<<' '
          <<"is"<<' '
          <<"my_value/div_num"<<' '<<''''<<my_value/div_num<<''''<<' '
          <<std::endl;

 // output: "my_value= '26.000000' divided by div_num= '5' is my_value/div_num= '5.200000' \n"


 \endcode

 @remarks INFO delibberately is designed to avoid custom formatting to keep it as simple as possible.
          If really needed you can still do custom formatting for a certain type of object by defining a operator<< .
          Or sometimes fall back to ordinary printing. Developers are in a hurry when debugging, they hate typing more
          then stricktly required.


*/
#  if 1
#     define INFO(...) util::info(__VA_ARGS__)
#  else
/// disabled: better then printf easy to use (with format specifier) printing for the sole purpose of debugging
# define INFO(...)
#  endif


//:DBG_INFO
#  if DEBUG
/// DEBUG depended (enabled) better then printf easy to use (with format specifier) printing for the sole purpose of debugging
/// @sa INFO
#    define DBG_INFO(...) INFO(__VA_ARGS__)
#  else
/// DEBUG depended (enabled) better then printf easy to use (with format specifier) printing for the sole purpose of debugging
#    define DBG_INFO(...)
#  endif

#  if ALLOC_DEBUG
/// ALLOC_DEBUG dependend (enabled) better then printf easy to use (with format specifier) printing for the sole purpose of debugging
/// @sa INFO
#    define ALDBG_INFO(...) INFO(__VA_ARGS__)
#  else
/// ALLOC_DEBUG dependend (enabled) better then printf easy to use (with format specifier) printing for the sole purpose of debugging
#    define ALDBG_INFO(...)
#  endif





//:VARVAL
/// gives expression list of 2 strings, the first is the variable name the second its natuarally streamed value
#  define VARVAL(_v) #_v"=",util::single_quote(_v)

//:VARVALHEX
/// as VARVAL but the value (if nummeric) is presented hexadecimally
#  define VARVALHEX(_v)  #_v"=",util::single_quote(_v,/*hex=*/true)

//:PTRVAL
/// gives expression list of 2 strings, the first is the pointer name the second its std::ostream pointer representation
#  define PTRVAL(_p) #_p"=",util::single_quote(reinterpret_cast<const void*>(_p)),"->",((_p)?util::single_quote(*_p):"(nullptr)")

//:PTRVALHEX
/// As PTRVAL but the pointed value is represented exadecimal
#  define PTRVALHEX(_p) #_p"=",util::single_quote(reinterpret_cast<const void*>(_p)),"->",((_p)?util::single_quote(*_p,/*hex=*/true):"(nullptr)")


//:VARCHRNUM
/** @brief byte specific VARVAL, represent byte as number not as character

 (u)int8_t types tend to be printed as a character by the default type overloading.
 By casting it to unsigned we select the number printing overload of ostream
 */
#  define VARCHRNUM(_v) #_v"=",util::single_quote(CHAR2UINT_CAST(_v))


//:VARCHRHEXNUM
/// Like VARCHRNUM but the byte is hexadecimally formatted
#  define VARCHRNUMHEX(_v) #_v"=",util::single_quote(CHAR2UINT_CAST(_v),/*hex=*/true)

#define INFO_STREAM_PTR (util::OstreamPtrRef())

#endif // INFO_HPP_ 1


