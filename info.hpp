#pragma once
//info.hpp
#ifndef INFO_HPP_
#  define INFO_HPP_ 1

#  include "util.hpp"

//@{
/**
 * @brief custom formatters that display the symbol or expression allong with the value.
 *
 * Usage INFO(VARVAL(value)); ,.... INFO(PTRCHRNUMHEX(address));
 */
//:VARVAL
/// gives expression list of 2 strings, the first is the variable name the second its natuarally streamed value
#  define VARVAL(_ARG) #_ARG"=",\
    tosics::util::custom_quote(_ARG)

//:VARVALHEX
/// as VARVAL but the value (if nummeric) is presented hexadecimally
#  define VARVALHEX(_ARG)  #_ARG"=",\
    ( tosics::util::AppliedInfoSettingsPtr->next_hex(),tosics::util::custom_quote(_ARG) )

//:VARCHRNUM
/** @brief byte specific VARVAL, represent byte as number not as character

 (u)int8_t types tend to be printed as a character by the default type overloading.
 By casting it to unsigned we select the number printing overload of ostream
 */
#  define VARCHRNUM(_ARG) #_ARG"=",\
    tosics::util::custom_quote(CHAR2UINT_CAST(_ARG))

//:VARCHRNUMHEX
/// Like VARCHRNUM but the byte is hexadecimally formatted
#  define VARCHRNUMHEX(_ARG) #_ARG"=",\
    ( tosics::util::AppliedInfoSettingsPtr->next_hex(2),tosics::util::custom_quote(CHAR2UINT_CAST(_ARG)))


#define DEREF_OBJECT
#ifdef DEREF_OBJECT  // requires operator << for *_ARG
//:PTRVAL
/// gives expression list of 4 strings, the first is the pointer name, the second its std::ostream pointer representation, 3th is -> and the 4th is the pointed value
#  define PTRVAL(_ARG) #_ARG"=",\
    tosics::util::custom_quote(reinterpret_cast<const void*>(_ARG)),\
    "->",\
    (tosics::util::Is_null(_ARG)? "(nullptr)" \
                     : tosics::util::custom_quote(*_ARG) )

//:PTRVALHEX
/// gives expression list of 4 strings, the first is the pointer name, the second its std::ostream pointer representation, 3th is -> and the 4th is the pointed value as hexadecimal
    #  define PTRVALHEX(_ARG) #_ARG"=",\
    tosics::util::custom_quote(reinterpret_cast<const void*>(_ARG)),\
    "->",\
    (tosics::util::Is_null(_ARG)? "(nullptr)" \
                     : (tosics::util::AppliedInfoSettingsPtr->next_hex(),tosics::util::custom_quote(*_ARG)) )

//:PTRCHRNUMHEX
/// Combines PTRVAL and VARCHRNUMHEX (pointed character byte value is represented hexadecimal)
#  define PTRCHRNUMHEX(_ARG) #_ARG"=",\
    tosics::util::custom_quote(reinterpret_cast<const void*>(_ARG)),\
    "->",\
    (tosics::util::Is_null(_ARG)? "(nullptr)" \
                     : ( tosics::util::AppliedInfoSettingsPtr->next_hex(2),tosics::util::custom_quote(CHAR2UINT_CAST(*_ARG))) )

#else // no DEREF_OBJECT
//:PTRVAL
/// gives expression list of 2 strings, the first is the pointer name the second its std::ostream pointer representation
#  define PTRVAL(_ARG) #_ARG"=",\
    tosics::util::custom_quote(reinterpret_cast<const void*>(_ARG))

//:PTRVALHEX
/// As PTRVAL but the pointed value is represented hexadecimal
#  define PTRVALHEX(_ARG) #_ARG"=",\
    tosics::util::custom_quote(reinterpret_cast<const void*>(_ARG))

//:PTRCHRNUMHEX
/// Combines PTRVAL and VARCHRNUMHEX (pointed character byte value is represented hexadecimal)
#  define PTRCHRNUMHEX(_ARG) #_ARG"=",\
    tosics::util::custom_quote(reinterpret_cast<const void*>(_ARG))
#endif

//:PTRADR
/// gives expression list of 2 strings, the first is the pointer name the second its std::ostream pointer representation
#  define PTRADR(_ARG) #_ARG"=",\
    tosics::util::custom_quote(reinterpret_cast<const void*>(_ARG))



//:VARVALS:// i.s.o INFO(VARVAL(a),VARVAL(b),VARVAL(c) you can also write INFO(VARVALS(a,b,c))
#define VARVALS(...) tosics::util::varvals(VSTRINGS(__VA_ARGS__),__VA_ARGS__)

//@}


//@{
/// @brief macros debending on preprocessor definitions DEBUG and ALLOC_DEBUG

//:DBG_INFO
#  if DEBUG
/// DEBUG depended (enabled) better then printf easy to use (with format specifier) printing for the sole purpose of debugging
/// @sa INFO
#    define DBG_INFO(...) INFO(__VA_ARGS__)
#    define DBG_INFO_FUNC INFO_FUNC
#  else
/// DEBUG depended (enabled) better then printf easy to use (with format specifier) printing for the sole purpose of debugging
#    define DBG_INFO(...)
#    define DBG_INFO_FUNC
#  endif

#  if ALLOC_DEBUG
/// ALLOC_DEBUG dependend (enabled) better then printf easy to use (with format specifier) printing for the sole purpose of debugging
/// @sa INFO
#    define ALDBG_INFO(...) INFO(__VA_ARGS__)
#  else
/// ALLOC_DEBUG dependend (enabled) better then printf easy to use (with format specifier) printing for the sole purpose of debugging
#    define ALDBG_INFO(...)
#  endif
//@}

//:INFO
/**
 @brief enabled: Lets you quicky and systematically print variables. Better for debugging then standard printing.
 @param ... expects any amount of objects that can be streamed to std::ostream.
 @sa VARVAL() PTRVAL() VARVALHEX() VARCHRNUM() VARCHRHEXNUM()

 \code INFO( a, b, c) \endcode
 does the same as
 \code std::cout << a << ' ' << b << ' ' << c << ' ' << std::endl \endcode
 but with less typing. It is intended to be used with macro's aka VARVAL,PTRVAL... as a low level and low entry
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

 // output: "my_value= '26.000000' divided by div_num= '5' is my_value/div_num= '5.200000'\n"


 \endcode

 @remarks INFO delibberately is designed to avoid custom formatting to keep it as simple as possible.
          If really needed you can still do custom formatting for a certain type of object by defining a operator<< .
          Or sometimes fall back to ordinary printing.
          Developers are in a hurry when debugging, they hate typing more then stricktly required, they should choose
          INFO to display variables e.t.c. i.s.o. plain printf or cout<< . The other application is to fabricate
          strings for embedded languages (aka a specific interpreter, html or embedded (dynamic) sql).
*/

//:INFO_FUNC:// Fast easy display function
#define INFO_FUNC INFO(BOOST_CURRENT_FUNCTION)

/** @note None of the util functions below should be used directly!
 * Use the macro's enlisted above (start reading about INFO)
 */

namespace tosics::util {

//:onullstream:// For providing s ostream& that supresses writing (aka /dev/null)
    class
onullstream : public std::ostream
{
    class : public std::streambuf
    {
      public:
        int overflow(int _c) override final
        {
            return _c;
        }

    }   hole;

  public:
    using std::ostream::ostream;

    static onullstream* SinkPtr() {
        static onullstream TheSinkInstance;
        return &TheSinkInstance;
    }


    onullstream()
    : onullstream(&hole)
    {
    }
};

  /// @brief Data or settings to support the INFO system, alling easy change and extension
  class InfoSettings
  {
    public:
      //@{ configurable
      std::ostream* ostreamPtr= &std::cout;
      const char* objectSeparation= " ";
      const char* pairItemsSeparation= ":";
      const char* customQuote= "'";
      const char* containerStart="{";
      const char* containerItemSeparation=",";
      const char* containerEnd="}";
      const char* tupleStart="(";
      const char* tupleItemSeparation=",";
      const char* tupleEnd=")";                         //TODO: see util.cpp:InfoSettings::validateFailed()
      //@} configurable

      // work variables
      short objOnLineCnt= 0;
      short hex_once= false;

      /// @brief Perform some basic checks that ensure that following INFO will perform valid output.
      /// @return 0 if all is OK, >0 OK but unusual, <0 if not OK to be used with STATEREPORT
      state_t validateFailed() const;

      /// @brief for macros to display a hexadecimal value, 0: not hex  >0: hex, value to set with  <0: hex
      void inline next_hex(short _width=-1)
      {
          hex_once= _width;
      }
  };

  extern thread_local InfoSettings *AppliedInfoSettingsPtr;  // implemented in util.cpp

//@{
/// @brief Basic low level definitions

// Explicitly defining how objects are separated can be used to customize
// formatting and debugging the 'info' module.
    inline char const*
ObjectSeparation()
{
    return AppliedInfoSettingsPtr->objectSeparation;
}
    inline char const*
PairItemsSeparation()
{
    return AppliedInfoSettingsPtr->pairItemsSeparation;
}

    inline char const*
CustomQuote()
{
    return AppliedInfoSettingsPtr->customQuote;
}

    inline char const*
ContainerStart()
{
    return AppliedInfoSettingsPtr->containerStart;
}

    inline char const*
ContainerItemSeparation()
{
    return AppliedInfoSettingsPtr->containerItemSeparation;
}

    inline char const*
ContainerEnd()
{
    return AppliedInfoSettingsPtr->containerEnd;
}


    inline char const*
TupleStart()
{
    return AppliedInfoSettingsPtr->tupleStart;
}
    inline char const*
TupleItemSeparation()
{
    return AppliedInfoSettingsPtr->tupleItemSeparation;
}
    inline char const*
TupleEnd()
{
    return AppliedInfoSettingsPtr->tupleEnd;
}


/*
 To count objects that are printed on the same line. By function encapsulation
 of the object we can have the counter in a header-only implementation. Generally
 global (static) variables are bad. Here it is somewhat improved by use of
 thread local storage. Also in forked processes and mpi based processes independed
 (threat) processes do not effect each other. It would be better to store
 context in a separate object and operate from there. That could be one of the
 future directions of info.
*/
    inline short&
ObjOnLineCntRef()
{
    return AppliedInfoSettingsPtr->objOnLineCnt;
}
    inline std::ostream*&
OstreamPtrRef()
{
    return AppliedInfoSettingsPtr->ostreamPtr;
}
//:INFO_STREAM_PTR
#  define INFO_STREAM_PTR (tosics::util::OstreamPtrRef())

    inline void
Redirect_info_to( std::ostream* target_)
{
    // We can not have *nullptr as ostream reference. Instead we refer to a onullstream object.
    // It is safe to write to it. But the net result is that there is no output.
    INFO_STREAM_PTR= target_ ?target_ :onullstream::SinkPtr();
}
//:INFO_TO
/// @brief redirect INFO to other stream
#  define INFO_TO(TargetOstream_) tosics::util::Redirect_info_to( & TargetOstream_ )


class NextLines
{
  // This type is to let specilized template function do_stream( NextLines)
  // issue a newline. The postfix '_tag' indicates that this class is used only
  // as a type selection mechanism by the compiler.
  // see void do_stream( NextLines)
  public:
    short count=0;

    NextLines(short _count=0)
    : count(_count)
    {}
};
//:ENDL
/// @brief issue newline and flush (aka << std::endl)
#  define ENDL tosics::util::NextLines(1)

//:ENDLINES
/// @brief issue newline and flush (aka << std::endl)
#  define ENDLINES(extra_newlines) tosics::util::NextLines(extra_newlines)

//@}
//______________________________________________________________________________

/*ENABLE IF THERE IS A << OPERATOR FOR Object_T */
    template  <
        typename Object_T,
        std::enable_if_t< is_to_stream_writable<std::ostream, Object_T const&>::value, bool> HasWrOpr= true
    >
    inline void
now_stream_object( std::ostream* os_ ,Object_T const& _object)
{
    ( *os_ )<< _object;
}
/*ELSE*/
    template  <
        typename Object_T,
        std::enable_if_t< not is_to_stream_writable<std::ostream, Object_T const&>::value, bool> HasWrOpr= false
    >
    inline void
now_stream_object( std::ostream* os_ ,Object_T const&)
{
    ( *os_ )<< RED <<"?"<<WHITE;
}
/*END ENABLE IF*/

    template<>
    inline void
now_stream_object<bool>( std::ostream* os_ , bool  const& _object)
{
    now_stream_object( os_, ( _object?"true":"false" ));
}
    template<>
    inline void
now_stream_object<std::nullptr_t>( std::ostream* os_ , std::nullptr_t const&)
{
    // std::nullptr_t is not a pointer so it needs its own specialization
    //FAKE_USE(_object);
    now_stream_object( os_, "nullptr");
}



//@{
/// @brief Specialize for floating point types because they need to presented with the correct precision.
    template  <typename Object_T>
    inline void
now_stream_object_fp( std::ostream* os_ , Object_T const& _object)
{
    (*os_) <<std::setprecision( std::numeric_limits<Object_T>::max_digits10) << _object;
}
    template<>
    inline void
now_stream_object<float>( std::ostream* os_ , float const& _object)
{
  now_stream_object_fp( os_, _object);
}
    template<>
    inline void
now_stream_object<double>( std::ostream* os_ , double const& _object)
{
  now_stream_object_fp( os_, _object);
}
    template<>
    inline void
now_stream_object<long double>( std::ostream* os_ , long double  const& _object)
{
  now_stream_object_fp( os_, _object);
}

//@}
//______________________________________________________________________________
//@{
    template<typename Value_T>
    inline std::string
custom_quote( Value_T const& _value)
{
    // alias to h1
    short& hx1= AppliedInfoSettingsPtr->hex_once;

    // before stream object
    std::ostringstream ss;
    ss << CustomQuote();
    char prior_fill=' ';
    if (hx1) {
        ss<< "0x";
        if ( hx1> 0 ) {
            prior_fill= ss.fill('0');
            ss<< std::setw( hx1);
        }
        ss<< std::hex;
    }

    now_stream_object( &ss, _value);

    // after stream object
    if (hx1) {
        ss<< std::dec;
        ss.fill(prior_fill);
        hx1= 0;
    }
    ss << CustomQuote();
    return /**/std::move(/**/ ss.str() /**/)/**/;
}
//@}
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
        ( *(OstreamPtrRef()) )<< ObjectSeparation();
    }
    ++(ObjOnLineCntRef());
    now_stream_object<Object_T>( OstreamPtrRef(), _object);
}
    template <>
    inline
void do_stream( NextLines const& _nextlines)
{
    ObjOnLineCntRef()=0;

    for( auto nextlines= _nextlines.count; nextlines>= 0; --nextlines ) {
#   if 0
        ( *(OstreamPtrRef()) )<< std::endl;    // flushes buffer
#   else
        (*OstreamPtrRef())<<'\n';              // does (possibly) not flush buffer
#   endif
    }//for
}
    inline void
info()
{
    ObjOnLineCntRef()=0;
    ( *(OstreamPtrRef()) )<< std::endl;    // flushes buffer
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
#define INFO(...) tosics::util::info(__VA_ARGS__)

    template <typename NextValue_T>
    void
argValues2StrVector( std::vector<std::string>* values_, NextValue_T const& _next_value_t )
{
    values_->push_back(custom_quote(_next_value_t));
}
//@{ VARVALS macro assistance
    template<typename FirstValue_T, typename... RemainingValues_T>
    void
argValues2StrVector( std::vector<std::string>* values_,
                          FirstValue_T const& _first_value, RemainingValues_T const&...  _remaining_values)
{
    argValues2StrVector( values_,_first_value);
    argValues2StrVector( values_,_remaining_values...);
}
    state_t
zipLeftsAndRightsJoin2Str( std::string *out_
                         , std::vector<std::string> const& _lefts
                         , char const* _left_right_separator
                         , std::vector<std::string> const& _rights
                         , char const* _zippedSeparator
                         );
void __varvals_merge_with_lables(std::string* out_, char const * _args_str, std::vector<std::string> const& _values);

    template <typename... VA_ARGS_T>
    std::string
varvals(char const * _args_str, VA_ARGS_T const&... _va_args)
{
    // extract values
    std::vector<std::string> values;
    argValues2StrVector( &values,  _va_args...);

    std::string return_value;
    __varvals_merge_with_lables( &return_value, _args_str, values);

    return return_value;
}
//@} VARVALS macro assistance



//@{ NEW DEVELOPMENT  genereric operator << to pretty printing (small) containers for all sequence containers
// (concept) requirements(CONTAINER_T _c) _c.begin(); _c.end(); CONTAINER_T::const_iterator::operator++()
//           NOTE: cbegin() and cend() would not work for
//          IMPORTAND: Use with any other type of containers (aka associative...) is undefined.
//

typedef   signed __int128  int128_t;
typedef unsigned __int128 uint128_t;

// Do NOT call the following function directly, only to be used from STREAM2STR,
// which creates a in place anominous ostringstream instance, hence it will not fail at runtime.
    inline std::string
$$$STREAM2STR_convert(const std::ostream& _ostringstreamWithMsgsOutputOperatorStream)
{
    auto stringstreamPtr= dynamic_cast<const std::ostringstream*>(&_ostringstreamWithMsgsOutputOperatorStream);

    if ( stringstreamPtr ) {
        // dynamic cast succeeded, ostrostringstream method calls) are possible
        return stringstreamPtr->str();
    }
    // otherwise (unlikely), show present static warning message
    // wrap this inline function in the STREAM2STR macro to guearantee, this will never happen.
    return "SEVERE WARNING: tosics::util::STREAM2STR_convert(_ostringstreamWithMsgsOutputOperatorStream) could not return intended message due to failing dynamic_cast!";
}

//:STREAM2STR
// Stream messages directly to a string, the responibility of allocating memory space for tyhe dynamic sized message is enirely deligated to ostringstream.
// example:
//   std::string ErrorMsg;  // Outer (global) scope declared.
//   ....later....  in a deeper scope.
//   ErrorMsg= STREAM2STR("Unable to set environment variable "<< ENV_CPX_VALIDATION_HASH);
//   // use the created message string
//   throw std::runtime_error(ErrorMsg);
//
#define STREAM2STR(_MSGS) tosics::util::$$$STREAM2STR_convert((std::ostringstream()<<_MSGS))

//:FUNC_MSG
// Similar to STREAM2STR but prefixes the message with source file and function/method location information
#define FUNC_MSG(_MSGS)  STREAM2STR("File:"<<__FILE__<<" function:"<<BOOST_CURRENT_FUNCTION <<" line:"<<__LINE__<<": "<<_MSGS)


//TODO: move to other include file that depends on info.hpp
    template<
        class _TargetT,
        class _OriginT,
        class _ExceptionT
        // class... 0<= remaining arguments for exception
    >
    _TargetT
$$$Dynamic_cast_orThrowException(_OriginT _origin, std::string _exceptionMessage /*,..*/ )
{
    _TargetT target= dynamic_cast<_TargetT>(_origin);
    if (target) {
        [[likely]]
        return target;
    }
    // otherwise
    std::string message= STREAM2STR("dynamic cast to "<< tosics::util::type_name<_TargetT>()<<' '<< \
     _exceptionMessage<<" . origin of type: "<<tosics::util::type_name<decltype(_origin)>( ));
    ThrowBreak(_ExceptionT(message));
}

// Provide as much as possible information at the source location
#define DYNAMIC_CAST_OR_THROWEXCEPTION(_TargetT,_origin,_ExceptionT,_message /*,...*/) \
    tosics::util::$$$Dynamic_cast_orThrowException<_TargetT,decltype(_origin),_ExceptionT> \
            (_origin,FUNC_MSG(_message << " . orgin: "<<#_origin<<"  ")/*,...*/)



//
// gcc 128 bit integer representation, fixed size (ffffffffffffffff:ffffffffffffffff) presesentation, as upper and lower 64 bit pair
//
// usage:
//   uint128_t my_128bitValue{0xFFF0FFF0FFF0FFF0FFF0FFF0FFF0FFF0uLL};
//   INFO(VARVAL(my_128bitValue));
//
//    ... ;

inline std::string hex128str(uint128_t const& _128bitValue) {
    std::ostringstream stream_which_returns_string;
    uint64_t upper64bit=  _128bitValue>> 64;
    uint64_t lower64bit=  uint64_t(_128bitValue);
    stream_which_returns_string
        << std::setw(16)<< std::setfill('0')<< std::hex<< upper64bit <<':'
        << std::setw(16)<< std::setfill('0')<< std::hex<< lower64bit
        ;
    return stream_which_returns_string.str();
}

    class
StringFrom128bitUInt // Simple class to manage the string memeory needed for the 128 bit representation
: public std::string
{
  public:
    StringFrom128bitUInt(uint128_t const& _u)
    : std::string(hex128str(_u))
    {
    }

    StringFrom128bitUInt(int128_t const& _u)
    : StringFrom128bitUInt(static_cast<uint128_t const&>(_u))
    {
    }
    StringFrom128bitUInt()
    : StringFrom128bitUInt(uint128_t(0))
    {
    }
};

    inline std::ostream&
operator << ( std::ostream& os_, uint128_t _u128 )
{
    // Outputs the std::string part of the temorary StringFrom128bitUInt object
    os_<< StringFrom128bitUInt(_u128);
    return os_;
}


// Allows representing pairs and thereby also maps can be represented (as containers of pairs).
    template<
        typename OS_T,
        typename T1,
        typename T2
    >
OS_T& operator << (OS_T& os_, std::pair<T1,T2> _pair)
{
    return os_ << _pair.first<< PairItemsSeparation() <<_pair.second;
}
//:for_each_alternate
//like for_each, but "alternates" the action with another action only and always between the foreach action
//This is a very common case where the alternation is a separation between the other action (aka:
//comma separated values).
//modified algorithm, body origin: /usr/include/c++/6/bits/stl_algo.h:for_each()
  template<typename _InputIterator, typename _Function, typename _AlternateFn>
    _Function
    for_each_alternate(_InputIterator __first, _InputIterator __last, _Function __f, _AlternateFn __af)
    {
      // concept requirements
      __glibcxx_function_requires(_InputIteratorConcept<_InputIterator>)
      __glibcxx_requires_valid_range(__first, __last);


      if ( __first!=__last ) {
        for(;;) {
            __f(*__first);
            ++__first;
            if ( __first == __last ) {
                break;
            }
            //else: alternate actions between actions of __f
            __af(*__first);
        }
      }

      return _GLIBCXX_MOVE(__f);
    }
namespace { // anonimous namespace to only use what is defined here in this module / this file
// Implementation body. Some containers require a different template signature.
    template<
        typename OS_T,
        typename ITERATOR_T
    >
    OS_T&
_operator_shiftleft_body(OS_T& os_,  ITERATOR_T const& _begin, ITERATOR_T const& _end)
{
    os_<< ContainerStart();
    for_each_alternate( _begin, _end,
        [&os_]/*print item*/(auto const& _item) {
            os_ << _item;
        },
        [&os_]/*alternate with printing item separation*/(auto const&) {
            os_ << ContainerItemSeparation();
        }
    );
    os_<< ContainerEnd();
    return os_;
}



    template<
        typename OS_T,
        typename CONTAINER_T
    >
    OS_T&
_operator_shiftleft_body(OS_T& os_,  CONTAINER_T const& _container )
{
    return _operator_shiftleft_body(os_ , _container.begin(), _container.end());
}

    template<
        typename OS_T,
        typename TU_T,
        size_t... I
    >
    void
_print_tuple(OS_T& os_, TU_T const& _tup, std::index_sequence<I...>) // requires C++17 index_sequence and folding
{
    os_ << TupleStart();
    (..., (os_ << (I == 0? "" : TupleItemSeparation()) << std::get<I>(_tup)  ));
    os_ << TupleEnd();
}
}//namespace
//_____________________________________________________________________________
    template<
         typename OS_T,
         template<
             typename _T,
             typename ..._R
         > typename CONTAINER_T,
         typename CharT,
         typename... R,
         typename Traits = std::char_traits<CharT>,
         typename Allocator = std::allocator<CharT>,
         typename std::enable_if_t<
                       ! std::is_same_v< CONTAINER_T<CharT,R...>, std::basic_string<CharT,Traits,Allocator> >
                      && is_iterable_v<  CONTAINER_T<CharT,R...> >
                       , int> = 0
    >
    OS_T&
operator <<  (OS_T& os_,  CONTAINER_T<CharT,R...>const& _container )
{
    return _operator_shiftleft_body( os_ , _container);
}

//_____________________________________________________________________________
    template<
        typename OS_T,
        typename T,
        std::size_t N
    >
    OS_T&
operator << (OS_T& os_,  std::array<T,N>const& _container   /* TODO: enable if it is a bounded std::array*/)
{
    return _operator_shiftleft_body( os_ , _container);
}

#if 1
/*
 operate with normal arrays, however char[] is considered the representation for C terminated C strings.
 We have to exclude char[] arrays. It becomes more comlex when 16 bit or 32 bit character codes are used.
 We do not need this, we could use std::array which is is almost the same and has some advantages.
 But the real challange is to get the signature for a native array right. That is the
 " T const (&_container)[N]" part.
 With upcomming binary streaming operators ( using < and > ), it is no issue.

 FIXME: Also a std::string is interpreted as an array and  i.s.o "word" "{w,o,r,d}" is printed.
        Use string.c_str() to as arround. Resolbe by ecluding string.
 */


    template<
        typename OS_T,
        typename T,
        std::size_t N,
        typename std::enable_if_t<
                      ! std::is_same_v<T,char>
                     && std::is_array_v<T[N]>
                      , int> = 0
    >
    OS_T&
operator << (OS_T& os_, T const (&_container)[N] )
{
    return _operator_shiftleft_body( os_ , std::begin(_container), std::end(_container));
}
#endif

    template<
        typename OS_T,
        typename ... T
    >
    OS_T&
operator << (OS_T& os_, std::tuple<T...> const& _tup)
{
    _print_tuple( os_, _tup, std::index_sequence_for<T...>{});
    return os_ ;
}




//////////////////////////////////////////////////////////////////////////////////////////////////////////
extern uint32_t LittleEndianCrc32Table[256];

class BitShifter
{
    uint64_t m_shiftbuffer=0;
    uint16_t m_bitshiftCnt=0;

  public:
    /*
     *  x << y may actually be implemented as x << ( y % (MAX_SHIFT_OF_BITS+1) )
     *  For example x << 64 can do x << ( 64 % 64 ) which is x << 0.
     *  This is not desired here. Any shift more then MAX_SHIFT_OF_BITS will
                                                                                                                                                                                                                *  remove all the bits.
                                                                                                                                                                                                                *
                                                                                                                                                                                                                *  Practically, when _bits is 64 or more, assume that all bits (aka, the internal buffer) is transfered.
                                                                                                                                                                                                                *  See if ( EXPECT_false_FROM(_bits>MAX_SHIFT_OF_BITS)) ... in inShiftLeft() and outShiftRight().
     */

    static constexpr uint16_t MAX_SHIFT_OF_BITS = ( sizeof m_shiftbuffer ) * 8 -1;

        static uint16_t
    MaxBitsToShiftOutRight(uint64_t _bits_from) // count shift operations needed to remove any bit bit set from _bits_from
    {

        uint16_t count_shift_1bit_right_operations=0;
        while ( _bits_from ) {
            ++count_shift_1bit_right_operations;
            _bits_from >>=1ull;
        }
        return count_shift_1bit_right_operations;
    }

    BitShifter(uint64_t  const& _shiftbuffer=0ull)
    : m_shiftbuffer(_shiftbuffer)
    {
    }
        uint64_t
    shiftbuffer() const
    {
        return m_shiftbuffer;
    }
        void
    shiftbuffer(uint64_t const& _shiftbuffer)
    {
        m_shiftbuffer= _shiftbuffer;
    }
        void
    update_bitshiftCnt() // set to value that allows to right shift out all bits currently set
    {
        m_bitshiftCnt = MaxBitsToShiftOutRight(m_shiftbuffer);
    }
        uint16_t
    bitshiftCnt() const
    {
        return m_bitshiftCnt;
    }
        void
    inShiftLeft(uint64_t const& _value, uint16_t const&  _bits)
    {
        if ( EXPECT_false_FROM(_bits > MAX_SHIFT_OF_BITS) ) {
            m_bitshiftCnt= MAX_SHIFT_OF_BITS+ 1;
            m_shiftbuffer= _value;
            return;
        }
        m_bitshiftCnt+= _bits;
        ( m_shiftbuffer<<= _bits )|= ( _value & Mask(_bits) );
    }
        uint64_t
    outShiftRight(uint16_t _bits)
    {
        if ( EXPECT_false_FROM( _bits > MAX_SHIFT_OF_BITS ) ) {
            decltype(m_shiftbuffer) out = m_shiftbuffer;
            m_shiftbuffer= 0;
            m_bitshiftCnt= 0;
            return out;
        }
        if ( _bits> m_bitshiftCnt ) {
            _bits= m_bitshiftCnt;
            m_bitshiftCnt= 0;
        }
        else {
            m_bitshiftCnt-= _bits;
        }

        decltype(m_shiftbuffer) out= m_shiftbuffer & Mask(_bits);
        m_shiftbuffer>>= _bits;
        return out;
    }
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * StrongIdentifier, creates a hash based on SHA1 but with extra requirements on the resulting hash.
 * 1. The hash result can be used as a identifier name.
 * 2. The hash uses the full and possible range of characters, 62 in total A-Z,a-z,0-9 and A-Z,a-z for the first characrer.
 *    It is far sorter than a SHA1 hash. Binary 0-9 are encoded as 0-9 digits.
 * 3. The SHA1 delivers 160 bits, plus a crc32 algorithm delivers 32 bits, then these 196 bits are compressed into 32 characters.
 * 4. Use of differentiating counters for specific indexes to harden the identifier uniqueness.
 * 5. No hash can guarantee full 100% uniqueness. But StrongIdentifier has been field tested against the complete filesystem
 *    of the test system to provide unique hashes for every file that has unique content, at least as good or better then SHA1.
 *
 * 6. OMMISSION: Currently. It can only be used once, there is no reset or restart build in.
 *    You will need no create/initialize/construct a new object. This is usually no problem,
 *    because it should be declared locally on the stack.
 */



class SHA1 //StrongIdentifier /* SHA1CrcEnc64Id */  // customized for use in tosics, covering common cases
: public sha1::SHA1
{
    // NOTE: Numbers used are non-magic, they rather are psysical fixed values, do not change/parameterize them.

    using sha1::SHA1::SHA1;
    using sha1::SHA1::processBytes;

    constexpr static char NUMS_0_63_TO_CHAR[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_-+?";
    constexpr static unsigned MODULO_VALUE= 10 + 26 + 26;  // (count 0-9) + (count A-Z) + (count a-z)

    uint32_t m_heur=0;      // sum or might be other heuristics value, by default litle endian crc32
    char m_outputbuffer[40];// size is 8 byte aligned

  public:
    //SHA1() {}

  private:
        void
    process6uint32wordsTo37Chars(
                        uint32_t const& _word11, uint32_t const& _word12, uint32_t const& _word13,
                        uint32_t const& _word21, uint32_t const& _word22, uint32_t const& _word23)
    {
        // The first and last 4 characters are self check values so that you can do
        // fast unequivalence checks aka (following psuedo code):
        //    if ( a[0] != b[0] ) return false;
        //    if ( a[33,34,35,36] != a[33,34,35,36] ) return false
        //    return a[1..32]==b[1..32]
        // If those characters do not match, then characters 1..33 can be the same,
        // generated from another origin.

        BitShifter bs;
        char *pUnc64=m_outputbuffer+3; // only lambda encode6BitsToChar is allowed to alter this value
        char const* outputbuffer_max_last= m_outputbuffer+ ( sizeof m_outputbuffer )- 1;  // 3 char positions reserved
        uint64_t is62cnt=0,is63cnt=0, has3bitsCnt=0, otherCnt=0;
        uint64_t sum=0;

            auto
        index2pos=[this,pUnc64]() -> uint64_t
        {
            uint64_t tpos= pUnc64-m_outputbuffer;
            while (tpos>=MODULO_VALUE) tpos-= MODULO_VALUE;
            return tpos;
        };

            auto
        encode6BitsToChar=[&]()
        {
            // avoid m_outputbuffer overflow, any remaining encodig values are simply cut of and lost, but this is very unlikely to happen
            if ( EXPECT_false_FROM(pUnc64>= outputbuffer_max_last) ) return;

            uint64_t pre_index = bs.outShiftRight( 6), index;
            sum+=pre_index;
// pragma disable -Wimplicit-fallthrough=
            switch (pre_index) { // to determine index and manage counters
                case  7: case 11: case 13: case 14: case 19:
                case 21: case 22: case 25: case 26: case 28:
                case 35: case 37: case 38: case 41: case 42:
                case 44: case 49: case 50: case 52: case 56:
                    // count the values that each have 3 bits set and 3 bits cleard
                    ++has3bitsCnt;
                    index = pre_index;
                    break;

                case MODULO_VALUE:
                    ++is62cnt;
                    index = /* 0 + */index2pos();
                    break;

                case MODULO_VALUE+1:
                    ++is63cnt;
                    index = MODULO_VALUE- index2pos();
                    break;

                default:
                    ++otherCnt;
                    index = pre_index;
            }
// enable
            *pUnc64++ = NUMS_0_63_TO_CHAR[ index];

            // ASSERT(index<sizeof NUMS_0_63_TO_CHAR);
        };


        *m_outputbuffer = 'X'; // will be replaced at the end by selection with firstindex
#if 1 /*test, 1 to check tail termination in debugger, otherwise to be set to: #if 0 .*/
        memset(m_outputbuffer, 'X', ( sizeof m_outputbuffer ));
#endif
        // 3 * 32 bits = 96 bits shiffed in, 6 bits * 16 shifted out
        sum=0;
        bs.inShiftLeft( _word11, 32);
        encode6BitsToChar();  encode6BitsToChar();  encode6BitsToChar();  encode6BitsToChar();  encode6BitsToChar(); // 2 bits remaining
        bs.inShiftLeft( _word12, 32);
        encode6BitsToChar();  encode6BitsToChar();  encode6BitsToChar();  encode6BitsToChar();  encode6BitsToChar(); // 4 bits remaining
        bs.inShiftLeft( _word13, 32);
        encode6BitsToChar();  encode6BitsToChar();  encode6BitsToChar();  encode6BitsToChar();  encode6BitsToChar(); // 6 bits remaining
        encode6BitsToChar();  // 0 bits remaining

        // again, so in total it becomes: 2 * 96 is 192 bits in total fit in 2*16 = 32 chrs
        bs.inShiftLeft( _word21, 32);
        encode6BitsToChar();  encode6BitsToChar();  encode6BitsToChar();  encode6BitsToChar();  encode6BitsToChar();
        bs.inShiftLeft( _word22, 32);
        encode6BitsToChar();  encode6BitsToChar();  encode6BitsToChar();  encode6BitsToChar();  encode6BitsToChar();
        bs.inShiftLeft( _word23, 32);
        encode6BitsToChar();  encode6BitsToChar();  encode6BitsToChar();  encode6BitsToChar();  encode6BitsToChar();
        encode6BitsToChar();
        ASSERT( bs.shiftbuffer()==0 );

        // Now the created profile by the sum of the all the encoded 6bit values (in range of full 0..63) and the
        // counters when exceeded 62 is added to the begin of the output buffer. This makes the code more distinct
        // from hash code clashes caused by re-encoding to position depended values. The sum can represented by
        // 11 bits (34*63=1142, 2^11=2048).
        uint64_t profile= ( sum<< 3 )+ ( is62cnt<< 2 ) + is63cnt;
        bs.shiftbuffer( profile );
        bs.update_bitshiftCnt();
        sum= 0;
        encode6BitsToChar();encode6BitsToChar();

        // Determine 1st character, this can only be A..Z,a..z but on 0..9
        constexpr static unsigned FIRSTCHAR_MODULO_VALUE= MODULO_VALUE- 10;
        uint64_t firstindex = 10+ ( bs.shiftbuffer()<< 6 )+ ( is63cnt<< 1 )+ is62cnt;
         //INFO(VARVALS(is62cnt,is63cnt,sum,profile,bs.shiftbuffer(),firstindex));
         // reduce value till its in range of [10...]MODULO_VALUE
         while ( firstindex >= MODULO_VALUE ) firstindex -= FIRSTCHAR_MODULO_VALUE;
         ASSERT( firstindex>= 10 ); ASSERT( firstindex<= MODULO_VALUE );

        ASSERT( pUnc64<= ( m_outputbuffer + ( sizeof m_outputbuffer )- 3 ) );
        ASSERT( has3bitsCnt < MODULO_VALUE && otherCnt < MODULO_VALUE );

        uint64_t encodeCnt=has3bitsCnt+is62cnt+is63cnt+otherCnt;
         ASSERT( encodeCnt==34 ); // must excactly match number of calls to encode6BitsToChar()
         ASSERT( m_outputbuffer + 3 + encodeCnt == pUnc64 );

        // Store first 3 characters ah hash of hash,
        // if 1st 3 are same (nearly 200e3 possibilities - but not without collissions),
        // it is likely that rest is the same. So the first (3 or more) characters could
        // be used to split hashes in 2 parts (aka, 1st a directory, 2nd filename).
        m_outputbuffer[0] = NUMS_0_63_TO_CHAR[ firstindex ];
        m_outputbuffer[1] = NUMS_0_63_TO_CHAR[has3bitsCnt];
        m_outputbuffer[2] = NUMS_0_63_TO_CHAR[MODULO_VALUE - otherCnt];

        *pUnc64++ = '\0';
        *pUnc64++ = '\0';
        *pUnc64++ = '\0';

        ASSERT( 40== ( pUnc64- m_outputbuffer ) ); // should have reached the end of the buffer
        ASSERT( strlen(this->outputbuffer())==37 ); // should aways have 37 characters in total
    }
        void
    bussybox_crc32_block_endian0(const void* const _data, size_t _len)
    {
        // I dont need to invent crc32. I grabbed proven code from the bussybox project.
        // But only what I need. Thats the little endian table and block algorithm.
        // Dont try to use range based loop or std::for_each here, its already tried before.
        // It is in appropiate or causes boilerplate code.
        for(auto pDataByte= static_cast<uint8_t const*>(_data), end= pDataByte+_len;
            pDataByte< end; ++pDataByte) {
            uint8_t lecTableItemIndex(static_cast<uint8_t>(m_heur) ^ *pDataByte);
            m_heur = LittleEndianCrc32Table[ lecTableItemIndex] ^ ( m_heur>> 8 );
        }
    }

  public:
        void
    processBytes(const void* const _data, size_t _len)     // WARNING: designed not to overide sha1::SHA1::processBytes
    {
        (void) sha1::SHA1::processBytes(_data,_len);
        bussybox_crc32_block_endian0(_data, _len);
    }
        void
    processString( std::string _s )
    {
        this->processBytes( static_cast<void const*>( _s.data()), _s.length());
    }
        char const*
    make_digest_string()
    {
        sha1::SHA1::digest32_t digest;//array of int32 decays to pointer, which allows modification in getDigest() call.
        getDigest(/*output to*/digest);
        sprintf( m_outputbuffer, "%04x%04x%04x%04x%04x%04x",
                 digest[0],digest[1],digest[2],digest[3],digest[4],m_heur);
        return m_outputbuffer;
    }
        char const *
    make_fingerprint()
    {
        // When adding _startChr to the output in 'A-Z,a-z then the resulting output can be used as a variable name, not just as filename
        sha1::SHA1::digest32_t digest;
        sha1::SHA1::getDigest(/*output to*/digest);
        process6uint32wordsTo37Chars(digest[0], digest[1], digest[2], digest[3], digest[4], m_heur);
        return outputbuffer();
    }

    // Output property, for reuse output when already calculated. make_digest_string() or make_fingerprint() are to be called before.
    // otherwise the value is undefined.
        char const *
    outputbuffer()
    {
        return m_outputbuffer;
    }
};

//@} NEW DEVELOPMENT
}// namespace tosics::util
//@}




#endif // INFO_HPP_ 1


