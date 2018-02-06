#pragma once
//info.hpp
#ifndef INFO_HPP_
#  define INFO_HPP_ 1

#  include "util.hpp"

//@{
/** @note None of the util functions below should be used directly!
 * Use the macro's enlisted below (start reading about INFO)
 */

namespace tosics::util {

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
      bool hex_once= false;

      /// @brief Perform some basic checks that ensure that following INFO will perform valid output.
      /// @return 0 if all is OK, >0 OK but unusual, <0 if not OK to be used with STATEREPORT
      state_t validateFailed() const;

      /// @brief for macros to display a hexadecimal value
      void inline next_hex()
      {
          hex_once=true;
      }
  };

  extern thread_local InfoSettings *AppliedInfoSettingsPtr;  // implemented in utils.cpp

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

    inline void
Redirect_info_to( std::ostream* target_)
{
    OstreamPtrRef()= target_;
}

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
//@}
//______________________________________________________________________________

    template  <typename Object_T>
    inline void
now_stream_object( std::ostream* os_ ,Object_T const& _object)
{
    ( *os_ )<< _object;
}

    template<>
    inline void
now_stream_object<bool>( std::ostream* os_ , bool  const& _object)
{
    now_stream_object( os_, ( _object?"true":"false" ));
}
    template<>
    inline void
now_stream_object<std::nullptr_t>( std::ostream* os_ , std::nullptr_t  const& _object)
{
    // std::nullptr_t is not a pointer so it needs its own specialization
    FAKE_USE(_object);
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
    std::ostringstream ss;
    ss << CustomQuote();
    if (AppliedInfoSettingsPtr->hex_once) {
        ss<< "0x"<< std::hex;
    }
    now_stream_object( &ss, _value);
    if (AppliedInfoSettingsPtr->hex_once) {
        ss<< std::dec;
        AppliedInfoSettingsPtr->hex_once = false;
    }
    ss << CustomQuote();
    return /**/std::move(/**/ ss.str() /**/)/**/;
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

//:onullstream:// For providing s ostream& that supresses writing
    class
onullstream : public std::ostream
{
    using std::ostream::ostream;

    class : public std::streambuf
    {
      public:
        int overflow(int _c) override final
        {
            return _c;
        }

    }   hole;

  public:
    onullstream()
    : onullstream(&hole)
    {
    }
};


//@{ NEW DEVELOPMENT  genereric operator << to pretty printing (small) containers for all sequence containers
// (concept) requirements(CONTAINER_T _c) _c.begin(); _c.end(); CONTAINER_T::const_iterator::operator++()
//           NOTE: cbegin() and cend() would not work for

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
//like for_each, but "alternates" the action with another actiononly and always between the foreach action
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

      for(;;) {
        __f(*__first);
        if ( ++__first==__last ) {
            break;
        }
        //else: alternate actions between actions of __f
        __af(*__first);
      }

      return _GLIBCXX_MOVE(__f);
    }
namespace { // anonimous namespace to only use what is defined here in this module / this file
// Implementation body. Some containers require a different template signature.
    template<
        typename OS_T,
        typename CONTAINER_T
    >
    OS_T&
_operator_shiftleft_body(OS_T& os_,  CONTAINER_T const& _container )
{
    os_<< ContainerStart();
    for_each_alternate( _container.begin(), _container.end(),
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
    template<
         typename OS_T,
         template<
             typename _T,
             typename ..._R
         >
         typename CONTAINER_T,
         typename T,
         typename... R
    >
    OS_T&
operator <<  (OS_T& os_,  CONTAINER_T<T,R...>const& _container)
{
    return _operator_shiftleft_body( os_ , _container);
}
    template<
        typename OS_T,
        typename T,
        std::size_t N
    >
    OS_T&
operator << (OS_T& os_,  std::array<T,N>const& _container)
{
    return _operator_shiftleft_body( os_ , _container);
    return os_;
}
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
//@} NEW DEVELOPMENT
}// namespace tosics::util
//@}

//:INFO_TO
/// @brief redirect INFO to other stream
#  define INFO_TO(TargetOstream_) tosics::util::Redirect_info_to( & TargetOstream_ )

//:ENDL
/// @brief issue newline and flush (aka << std::endl)
#  define ENDL tosics::util::NextLines(0)

//:ENDLINES
/// @brief issue newline and flush (aka << std::endl)
#  define ENDLINES(extra_newlines) tosics::util::NextLines(extra_newlines)


//:INFO_STREAM_PTR
#  define INFO_STREAM_PTR (tosics::util::OstreamPtrRef())


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
    ( tosics::util::AppliedInfoSettingsPtr->next_hex(),tosics::util::custom_quote(CHAR2UINT_CAST(_ARG)))

//:PTRVAL
/// gives expression list of 2 strings, the first is the pointer name the second its std::ostream pointer representation
#  define PTRVAL(_ARG) #_ARG"=",\
    tosics::util::custom_quote(reinterpret_cast<const void*>(_ARG)),\
    "->",\
    (tosics::util::Is_null(_ARG)? "(nullptr)" \
                     : tosics::util::custom_quote(*_ARG) )

//:PTRVALHEX
/// As PTRVAL but the pointed value is represented hexadecimal
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
                     : ( tosics::util::AppliedInfoSettingsPtr->next_hex(),tosics::util::custom_quote(CHAR2UINT_CAST(*_ARG))) )

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
#  if 1
#     define INFO(...) tosics::util::info(__VA_ARGS__)
#  else
/// disabled: better then printf easy to use (with format specifier) printing for the sole purpose of debugging
# define INFO(...)
#  endif

//:INFO_FUNC:// Fast easy display function
#define INFO_FUNC INFO(BOOST_CURRENT_FUNCTION)

#define STREAM2STR(_MSGS) dynamic_cast<std::ostringstream&>((std::ostringstream()<<_MSGS)).str()
#define FUNC_MSG(_MSGS) dynamic_cast<std::ostringstream&>((std::ostringstream()<< \
    "File:"<<__FILE__<<" function:"<<BOOST_CURRENT_FUNCTION <<" line:"<<__LINE__<<": "<<_MSGS)).str()



#endif // INFO_HPP_ 1


