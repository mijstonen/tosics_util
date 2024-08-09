#pragma once
//util.hpp
////////////////////
#define BUILD_ALL 2
// // BUILD_ALL comment. BUILD_ALL must be at the first line so it can be changed very easily by a automated tool.
// Touch , to force a rebuild all without make clean, make changes to
// the BUILD_ALL macro. There is and should be no further use of it.The value does not matter,
// as long it changes, increase it by one each time you request a complete rebuild makes sense.

#include "stdinc.hpp"

#ifndef UTILS_HPP_
#define UTILS_HPP_ 1


#include "extra_type_traits.hpp"

// Apply preprocessor options in Cmake
// open configuration -> CMake -> Show advanced -> [x] Show Advanced Values
// aka CMAKE_CXX_FLAGS_DEBUG -O0 -ggdb3 -DDEBUG=1 -DFAST=0 -DALLOC_DEBUG=1


//:DEBUG
/** \def DEBUG Preprocessor switch.
 If defined and 1 all debug code gets enabled. This is particular true for utillity macros, functions and classes.
 DEBUG also controlls system preprocessor variable NDEBUG.
 If DEBUG=0 then NDEBUG is 1, if DEBUG=1 then NDEBUG is undefined.
*/

//:ALLOC_DEBUG
/** \def ALLOC_DEBUG Preprocessor switch.
 Controlls NodeField (in NodeFieldBuffer) allocation debugging expressions, specially for developping and testing.
*/

//:FAST
/** \def FAST Preprocessor switch.
 In some cases code has been optimized (with carefull testing and profiling) and replaced with code that is
 (or oughts to be) faster.
*/


//#define DEBUG 0

// If FAST==1 it is like driving without seatbelt. Some redundant code that improves robustness is disabled
// to give you the best possible raw speed at higher risc of some instabillity, due to misperception of assumptions
// during the design (of cause, I do NOT do such thing, ha ha).
// #define FAST  1

//:CHAR2UINT_CAST
/// always convert ASCI character nummerics to range 0..255
#define CHAR2UINT_CAST(_CHR) static_cast<unsigned int>(static_cast<unsigned char>(_CHR))

//:VSTRINGS
/// @brief The equvalent of __STRING but for a variable number of arguments
// VSTRINGS(a,b(c),d) produces "a,b(c),d"
# define VSTRINGS(...) #__VA_ARGS__




//:PACKED
/// incapsulate gcc specific attribute __packed__
#if 1
# define PACKED __attribute__ ((__packed__))
#else
# define PACKED
#endif

//:ITEMS_IN
/// get size of a native C array in number of items (capacity)
#define ITEMS_IN(_C_array)                               (sizeof _C_array/sizeof *_C_array)

//:LAMBDA_RETURN_RESULT_TYPE
// The lambda execution resturned result type, without needing to instantiate a functor
// see also its use in RunAndFinally
#define LAMBDA_RETURN_RESULT_TYPE(_LambdaT) decltype( std::declval<_LambdaT>().operator()())

//:ASSUME_USED
/// gnu custom attribute
#if defined(__GNUC__) || defined(__clang__)
#define ASSUME_USED [[gnu::used]]
#else
#define ASSUME_USED
#endif


namespace tosics::util {
//:Items_in
/// MT20180209 NEW: And here is a way to do it generically with templates
    template <size_t N, typename T>
    constexpr size_t
Items_in(T const (&)[N])
{
    return N;
}
    template <typename T>
    constexpr size_t
Items_in(std::initializer_list<T>& _t_list)
{
    return _t_list.size();
}

}//namespace tosics::util



//:END_ADR
/// get the first addres beyond the last possible item with given size, used in macro END_OF
#define END_ADR(_C_array,_size)                          (_C_array+_size)


//:END_OF
/// get the first addres beyond the last possible item, analog to stl container end()
#define END_OF(_C_array)                                 END_ADR(_C_array,ITEMS_IN(_C_array))

//:BEGIN_MAIN
/// Standard C++ scripting entry
#define BEGIN_MAIN int main(int _argc, const char* _argv[]){


//:END_MAIN
/// Standard C++ scripting exit
#define END_MAIN return 0;}


//:USTD
/// Standard C++ scripting used name spaces
#define USTD using namespace std; using namespace util


//:FORCE_INLINE
/// gcc specific optimization encapsulation, overriding the compiler and truely force inlingning of function or method
#if 0
#define FORCE_INLINE __attribute__((always_inline))
#else
#define FORCE_INLINE inline
#endif

//:assert_ptr
/// assert that a pointer is valid (not NULL) while assigning the pointer
#define assert_ptr(_p) (assert(p!=nullptr),p)

#if DEBUG
# ifdef NDEBUG
#  undef NDEBUG
# endif


//:ASSERT
/// enabled: control ASSERT macro's for pre and post condition checking
# define ASSERT(...) assert(__VA_ARGS__)


//:ASSERT_PTR
/// enabled: stop NULL pointer during a assignment usage example: ptr = ASSERT_PTR( make_ptr())
# define ASSERT_PTR(_p) (assert((_p)!=nullptr),(_p))


//:ASSERT_VAL
/// enabled: stop ZERO value during a assignment, usage example: v = ASSERT_VAL(make_v())
# define ASSERT_VAL(_v) (assert((static_cast<int>(_v))),(_v))


//:ASSERT_ALWAYS_DO
/**
 @brief enabled: assert action in debug mode but do the action (unchecked) also in non debug mode

 @param _action the '_action' must provide a int() or bool()

 If the assertion fails, the (failed) action is reported
 usage example: ASSERT_ALWAYS_DO(initialize_program())
*/
# define ASSERT_ALWAYS_DO(_action) \
  ((_action) \
   ? __ASSERT_VOID_CAST (0) \
   : __assert_fail (__STRING(_action), __FILE__, __LINE__, __ASSERT_FUNCTION))

// perform action when depending on ASSERT is enabled or not
# define DEBUG_EXPRESSION(_action) _action

#else // !DEBUG
# ifndef NDEBUG
#  define NDEBUG
# endif

/// disabled: control ASSERT macro's for pre and post condition checking
# define ASSERT(...)
// pass pointer unchecked

/// disabled: stop NULL pointer during a assignment usage example: ptr = ASSERT_PTR( make_ptr())
# define ASSERT_PTR(_p) (_p)

/// disabled: stop ZERO value during a assignment, usage example: v = ASSERT_VAL(make_v())
# define ASSERT_VAL(_v) (_v)


/// @brief enabled: assert action in debug mode but do the action (unchecked) also in non debug mode
// do action ignore result and but dont check, normal assert would not perform the action
# define ASSERT_ALWAYS_DO(_action) __ASSERT_VOID_CAST ((_action))

// perform action when depending on ASSERT is enabled or not
# define DEBUG_EXPRESSION(_action)

#endif  // DEBUG


//{@ MT20180226 new code. Create unique id.

#define STRINGIZE(x) STRINGIZE_SIMPLE(x)
#define STRINGIZE_SIMPLE(x) #x

#define CONCAT(first, second) CONCAT_SIMPLE(first, second)
#define CONCAT_SIMPLE(first, second) first ## second

#define MAKE_UNIQUE(PREFIX) CONCAT( PREFIX, CONCAT(__LINE__,CONCAT(_,__COUNTER__)))
#define AUTO_ID CONCAT( AUTO_ID_PREFIX_$_, CONCAT(__LINE__,CONCAT(_,__COUNTER__)))

// By convention prefix with file identifier in file scope by define and undef of AUTO_ID_PREFIX_$_
#define AUTO_ID_PREFIX_$_ file_identifier_$_
#undef  AUTO_ID_PREFIX_$_

//}@ MT20180226 new code




//:DROP_OUTPUT
/**
 @brief Indicate that a optional actual output is not needed by the caller.

 By convention I use pointers to pass outputs from callee to caller.
 Optional outputs are checked and if nullptr, the callee results
 are dropped, because the caller has no use for it.

 @b example
 \code
   int in;
   int out;


   // normal use
   in=19;
   square_root( &out,  in);


   in= -20;
   try {
       // see if possible
       square_root( DROP_OUTPUT,  in);
   }
   catch (...)
   {
   }
 \endcode

 @remark Not all outputs are optional, see source and source documentation
        non-optional outputs are ASSERT checked, so only for debugging


 By design methos and functions will pass a pointer where a output is required.
 Typically they have the following pattern

 \code
 func( typeA* optional_output_ , typeB const& _input )
 {
   ...
   if ( optional_output_ ) {
     *optional_output_ = ... ;
   }
 }

 // in caller

 typeB source=...;
 typeA destination;

 func( &destination, source );
 \endcode

 If the output is not required, we can call

 \code
 func( nullptr, source );   // OK but not self documenting
 \endcode
 Here is where macro DROP_OUTPUT is intended for, we call
 \code
 func( DROP_OUTPUT, source );  // OK and self documenting
 \endcode

 In the last code example the result of the function is ignored.
 Typically this is used when a function produces also other output (like returning a bool) or does  a desired (IO)
 operation.
 */
#define DROP_OUTPUT (nullptr)

//:util
/**
 @brief common utillity operations widely used in the source and often practical for any project

 These operations evolved over the years, not all of it might be usefull the specific project.
 util moves from project to project and more functionallity is gathered.
*/



    // NOTE: Deliberately restricted to (most usefull) foreground coloring.
    //       There are several ANSI console codes libraries for more sophisticated features.
// prefered for string literal concatination and use in (embedded) php
#  define NOCOLOR  "\033[00m"
#  define RED      "\033[00;31m"
#  define YELLOW   "\033[00;33m"
#  define GREEN    "\033[00;32m"
#  define CYAN     "\033[00;36m"
#  define BLUE     "\033[00;34m"
#  define MAGENTA  "\033[00;35m"
#  define WHITE    "\033[00;37m"
#  define HRED     "\033[01;31m"
#  define HYELLOW  "\033[01;33m"
#  define HGREEN   "\033[01;32m"
#  define HCYAN    "\033[01;36m"
#  define HBLUE    "\033[01;34m"
#  define HMAGENTA "\033[01;35m"
#  define HWHITE   "\033[01;37m"
#  define RESET    "\033[0m"
// When more colors are needed, but restrict to the minimum that is needed.
// eXtended colors, a few of 256 colors, see docs/colorcodes.txt for extending the set
#  define XGREENYELLOW "\033[38;5;154m"
#  define XLIGHTYELLOW3 "\033[38;5;187m"
#  define XLIGHTSTEELBLUE1 "\033[38;5;189m"
#  define DARKSEAGREEN1 "\033[38;5;193m"
#  define XLIGHTCYAN1 "\033[38;5;195m"
#  define XPINK1   "\033[38;5;218m"
#  define XWHEAT1  "\033[38;5;229m"
#  define XGREY35  "\033[38;5;239m"



namespace tosics::util {

//{ SCX   stands for System Const eXPression.
// Encapsulate system constants that trigger old-style-cast warnings.
// They are allowed here because we do not alter system files
// but no where else. THe name should be SCX_<orginal name>
//
//:SCX_MAP_FAILED
/// Same as MAP_FAILED but avoid old-style-cast warnings of the compiler, instantiated in util.cpp
extern const void* SCX_MAP_FAILED;
//} SCX

using int128_t  = signed   __int128;
using uint128_t = unsigned __int128;

// MT20170417, Some forward declarations/definitions, see statereport.hpp
//  defined state_t and stateLiteralArg_t are new and might not be used everywhere, using long and int should work well

//:state_t:// The CPU natural integer (can vary per processor, only use small(..est possible) values)
using state_t = long;

//:stateLiteralArg_t:// not passing a integer literal should be detected by the compiler (by using move semantics)
using stateLiteralArg_t = state_t&&;


//:State:// The intend is to make searches easy where a state is thrown or returned. Aka State(0) State(-1) State(12)
         // Also a great place to put in breakpoints when debugging.
    inline state_t
State(stateLiteralArg_t _state )
{
    return _state;
}

/**
@brief universal alignment function

@param[in]  _size_ The original size value
@param[in]  _min
@return     The minimum size that in which _size_ will fit, but is a hole number of times _min.

@post when dividing returnvalue by _min there will be no rest i.a.w.  returnvalue>=_size_ && returnvalue % min == 0

@b examples
\code
    AlignSizeToMultiMin( 475, 16); // (29+1)*16 = 480  480>475

    AlignSizeToMultiMin( 43, 12);     // non power 2 alignment returns 48

    AlignSizeToMultiMin( 1333,1024)  // returns 2048

    new_map_size= AlignSizeToMultiMin( new_map_size, mh.page_size); // make new_map_size aligned
\endcode
*/

size_t AlignSizeToMultiMin ( size_t _size_, size_t const& _min);


//:static_to_type_cast
/**
 @brief converts second argument to the type of the first argument, for maintainable mixed type operations.
 @tparam to_T    The target type
 @tparam from_T  The orgin type
 @param _toInstance The object belonging to the target type, note that the _toInstance content is @b NOT used.
 @param[in] _fromInstance The origin object to which the type conversion is applied.

 rationalle: Unfortunally (and sometimes quiet anoying) there a many different integer types. It frequently happens
 that these couse conversion warnings in conditionals and comparisson operations. The worst thing to do is to ignore
 or supress these warnings. There should be a proper check and a static_cast type conversion. But such conversion
 states the target type redundantly causing trouble when the target type changes. Because of relaxed C++11 typing like
 with  @e auto and @e decltype this problem is often non local and there is a urge to resolve it. static_to_type_cast()
 automatically adapts to (non-local) those changes as long the conversion remains valid.

 @remark One could argue "do not use different types, use only X". That is valid in a nice ideal brave world. But in
         practice there are many people, sources, libraries, packages and so on that use diffent approaches and they
         are mixed, combined and used. Sometimes more or less standard or ambigious.
         Type conversion so become fact of live. And this makes living a bit easier.

 @example
        tosics::util::setFileSize(...)
        {
        ...
            auto wr= write( _fd, "", 1);
            if ( wr< static_to_type_cast(wr,0) ) {
        ...
        }
 */
    template<typename to_T, typename from_T>
    inline to_T
static_to_type_cast(to_T _toInstance, from_T _fromInstance)
{
    return static_cast<decltype(_toInstance)>( _fromInstance);
}
    template <typename T>
    bool
Is_null (T _ptr)
{
    return ( _ptr ? false: true );
}

#if 0
template <>
inline
bool Is_null<std::nullptr_t>(std::nullptr_t _ptr)
{
    return true;
}
#endif

//:setFileSize
/**
 @brief set file to desired size with explicit grow or shrink control
 @param[in] _fd The file handle associated to the file of which the size has to change.
 @param[in] _size The size to which the file has to be set.
 @param[in] _grow (optional, default=true), if file will shrink @b false
            by default
 @sa int mmap_handler::resizeMap(size_t _newsize)
 @return 0 on success or <= -1 on error. Use STATEREPORT on call.

 This is a low level helper routine. If it needs to grow then the size is set by writing the last byte
 otherwhise the content beyond the set (shrinking) size is lost.

 @remark The caller needs to know wetter the file will shrink or grow. To do that the prior file size
         before calling setFileSize needs to be known.

 @remark File size setting is lseek(2) based. For that reason growing the size differs from shrinking the size.

 @sa man lseek(2)
*/
int SetFileSize ( int _fd, size_t _size, bool _grow=true);

//:FindIndex
/**
 @brief Find index (in reverse order) of strings in a array, to apply switch i.s.o long 'if else if' list.
 @param[in,out] _NumberOfItemsIn_indexOut_ in: Number of items in the array including the searched string (see below),
                out the index number found or 0 if not found.
 @param[in] _texts Array of strings to be found, additionally the search string should be on _texts[0].

 Sick of
         if ( !strmcmp(...) ) {
         } else if ( !strmcmp(...) ) { ...
         } else if ( !strmcmp(...) ) { ...
         } else if ( !strmcmp(...) ) { ...
         } else { ...
         }

         ?

  prefering form of
    switch( found ) {
      case "..."
      break;

      case "..."
      break;
      ....
  }

  ?

  1. create a (constexpr local c array) kind of list of items
  2. put the item to search for as first  (items[0])
  3  Declare a int and initialize from from ITEMS_IN(items)
  4  Call findIndex
  5  Use the result, probably in a switch


  use   When decision is to be made on a limited (probably statically defined) number of strings,
        aka (small) (programing) language parsing.

 @sa util_demos.cpp:findIndex_demo()

 @return 0 if _text[0] is not found in _texts[1]..._texts[ITEMS_IN(texts)-1]
           or 1..ITEMS_IN(texts)-1 when found.
*/
    inline void
FindIndex( int *_NumberOfItemsIn_indexOut_, const char* _texts[])
{
    ASSERT( ( *_NumberOfItemsIn_indexOut_ )> 0 );  // item to be found has to be on _texts[0]

    const char* item_to_be_found= _texts[0];
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wempty-body"  // for clang++
    while  (    strcmp( _texts[--( *_NumberOfItemsIn_indexOut_ )], item_to_be_found )
            &&  strcmp( _texts[--( *_NumberOfItemsIn_indexOut_ )], item_to_be_found )
            &&  strcmp( _texts[--( *_NumberOfItemsIn_indexOut_ )], item_to_be_found )
            &&  strcmp( _texts[--( *_NumberOfItemsIn_indexOut_ )], item_to_be_found )
           );
#pragma GCC diagnostic pop

    ASSERT( ( *_NumberOfItemsIn_indexOut_ )>= 0 );
}

//
// Would this be faster ? (by forward pre scanning by the processor)
// output: index+1 of found string (effectively the same as with FindIndex)
//         or 0 if not found.
// NOTE: The string to be found MUST be the last in _texts
//
    inline void
ForwardFindIndex( int *_NumberOfItemsIn_indexOut_, const char* _texts[])
{
    ASSERT( ( *_NumberOfItemsIn_indexOut_ )> 0 );// item to be found has to be on _texts[_NumberOfItemsIn_indexOut_-1]
    int numberOfItemsIn= *_NumberOfItemsIn_indexOut_;
    const char* item_to_be_found= _texts[ ( *_NumberOfItemsIn_indexOut_ )- 1 ];
    ( *_NumberOfItemsIn_indexOut_ )=0;
    while  ( strcmp( _texts[( *_NumberOfItemsIn_indexOut_ )++], item_to_be_found ) ){
        // DBG_INFO(VARVAL(_NumberOfItemsIn_indexOut_)); FAKE_USE(_NumberOfItemsIn_indexOut_);
    };
    if ( *_NumberOfItemsIn_indexOut_>= numberOfItemsIn ) {
        *_NumberOfItemsIn_indexOut_=0;
    }
}
//
// Due to caching, FindIndex() is best for searching in small sets of string literals, since
// _text[0] is causes cache line loading before the searching starts. Where as ForwardFindIndex would win in longer
// sets where the forward searching (over multiple cachelines) could win over a small portion of extra code.
// In practice it depends on the hardware. For large sets a binairy search algorithm works better.
//
// Note that FindIndex is well suited for membership testing logic  aka...
//
//  if ( x in abcde ) could be written as
//
//  if ( FindIndex( x, {"a","b","c","d","e"} ) {
//  }
//



char const* DateTime();

    void
Info_main_args (int argc, char const* argv[]);

#define SHOW_ARGS info_main_args(argc,argv)


enum eBreakCategory : int
/* examples, customize */
{ eBC_default,
    eBC_signal,
    eBC_io,
    eBC_ui,
    eBC_service,
    eBC_aync,
    eBC_assertion_failed,
    eBC_fatal,
    eBC_handled
};


//:TOSICS_UTIL_STACKTRACE
/// Provide std::ostream construct at the current source location to provide a stacktrace, to screen, file, string etc.
/// Works with STREAM2STR
#define TOSICS_UTIL_STACKTRACE "\n_____ S T A C K T R A C E _____\n"<< std::stacktrace::current() << "\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n"

extern std::vector<std::string> ProgramArguments; // Needed in ThrowBreak()



//@{ to be moved to separate unit
//:ThrowBreak:// Use it i.s.o. naked throw and put breakpoints in it and see full backtrace in debugger
    template <typename EXCEPTION_T>
    /* [[ noreturn ]] */ void
ThrowBreak(const EXCEPTION_T& _exception, eBreakCategory _break_category = eBC_default)
{
    const std::set<eBreakCategory> NoBacktraceCategory{ eBC_handled };

    if (NoBacktraceCategory.find(_break_category) == NoBacktraceCategory.end()) {

        // display enummeration name
        char const *break_category_name="eBC_?????";
#define throwBreak_EBC(_NAME) case _NAME: break_category_name= __STRING(_NAME); break
        switch (_break_category) {
            throwBreak_EBC(eBC_default);                 //
            throwBreak_EBC(eBC_signal);                  // put a breakpoint
            throwBreak_EBC(eBC_io);                      // on one of
            throwBreak_EBC(eBC_ui);                      // these throwBreak... lines
            throwBreak_EBC(eBC_service);                 //
            throwBreak_EBC(eBC_aync);                    //
            throwBreak_EBC(eBC_assertion_failed);        //
            throwBreak_EBC(eBC_handled);                 //
            throwBreak_EBC(eBC_fatal);                   //
        } //switch
#undef throwBreak_EBC




        std::cerr<< std::endl<< HRED
            << "executable:"<<ProgramArguments[0]<<" at: "<<__FILE__<<':'<<__LINE__<<": "<<__PRETTY_FUNCTION__
            << " break_category_name: "<< break_category_name<< NOCOLOR
            << std::endl
        ;
        std::cerr<< HRED<< TOSICS_UTIL_STACKTRACE<< NOCOLOR<<std::endl;
    } // find(_break_category)
    throw _exception;
}

//:ThrowEvent:// Use when a exception represents controlflow i.s.o. error.
    template <typename EVENT_EXCEPTION_T>
    [[ noreturn ]] void
ThrowEvent(EVENT_EXCEPTION_T event_exception_)
{
    // EXTENSION?: pass event to listners
    throw event_exception_;
}

    //:ASJ_special:// options for Append_splitted() and Append_joined() to handle custom situations
struct ASJ_special
{
    bool m_allow_empty=true;
    std::string m_up=  "([<{";
    std::string m_down=")]>}";
    std::string m_ignorers="'\"";
};

/*
   concept_BackAppendable
      CONTAINER_T must have member functions
         reference back()
         emplace_back(Args...)

    CONTAINER_T should be FowardIterable.
 */

//:Append_splitted:// For VARVALS implementation, to split string with arguments
//@{ 3 overloads

    template <typename CONTAINER_T = std::vector<std::string> >
    state_t
Append_splitted( CONTAINER_T* strs_, std::string const& _str
                       , char const* _seps = ","
#if 1  // 0 for clang workarround
                       , std::optional<ASJ_special> const& _optAsj=
                                                    std::optional<ASJ_special>()
#endif
               )
{
    // std::cout<<HBLUE<<"Taking optional ASJ"<<NOCOLOR<<std::endl;
    if (!strs_) {
        // strs_ output is mandatory and to be modified without further checks
        return State(-1);
    }
    char const*  pc = _str.c_str();
    if (!pc) {
        // pc must be valid to perform
        return State(-2);
    }
    if (!*pc) {
        // no input to split, but result is valid because nothing would change,
        // however: no input possibly is not intended
        return State(1);
    }

    char const*  start = pc; // [start...pc> is range of chars copied

    // control defaults
    bool allow_empty = true;
    char const* up   = "([<{";
    char const* down = ")]>}";
    char const* ignorers = "'\"";
    constexpr const char QUOTER = '\\';
    // optionally, initialize from control info
#if 1   // 0 for disable as workaround for clang
    if ( _optAsj ) {
        allow_empty = _optAsj->m_allow_empty;
        up = _optAsj->m_up.c_str();
        down = _optAsj->m_down.c_str();
        ignorers = _optAsj->m_ignorers.c_str();
    }
#endif
    if ( strlen(up)!= strlen(down) ) {
        // length of up and down need to be equal, they are not!
        return State(-5);
    }
    using index_type = decltype(strlen(""));
    index_type pair_index = strlen(up)+1;
    std::vector<index_type> pi_stack;  // Pair Index Stack

    //:find:// predicate to offset_ OF _item IN _items, returns true if found otherwise false
        auto
    find = [](index_type * offset_, char _item, char const* _items) -> bool
    {
        char const* found;
        found = strchr(_items, _item);
        if (found) {
            *offset_ = found - _items;
            return true;
        }
        return false;
    };


    //:maybe_append_strs_from_str_substr:// ..., when allow_empty is false then a empty substring is not added
        auto
    maybe_append_to_strs_from_str_substr = [&]() -> void
    {
        auto substr_len = pc - start;
        if ( allow_empty|| substr_len ) {

static_assert(is_appendable_at_end<CONTAINER_T>::value,
            "The used container should have methods emplace_back() and back");

            strs_->emplace_back();
            std::string& newStrRef= strs_->back();
            // Copy each char but without (single) QUOTER
            for( char const* i=start; i<pc; ++i ) {
                if ( QUOTER == *i ) {
                    if ( i<=start ) {
                        continue;
                    }
                    if ( QUOTER != *(i-1) ) {
                        continue;
                    }
                }
                ASSERT(*i);
                newStrRef+= *i;
            }
        }
    };

    //:next:// ignore split syntax within ignorers  (aka 'a,b(c,d),e' => 'a,b(c,d),e' i.s.o  a | b(c,d) | e)
        auto
    next=[ignorers](char const*& _pc_) -> state_t
    {
        for(unsigned cnt=0;*_pc_;++cnt) {
            char const* pIgnorer= strchr( ignorers, *_pc_);
            if ( !pIgnorer ) {
                if ( !cnt ) {
                    ++_pc_;
                }
                break;
            }

            // entering ignoring state, skip all till after terminator
            char terminator= *pIgnorer;
            do      {
                if ( !*++_pc_ ) {
                    return State(-1); // termination by '\0'  aka: some unterminated string / unmatched ignorer
                }
            }
            while (  ( *_pc_ )!= terminator || *( _pc_ -1 )== QUOTER );
            ++_pc_;
        }
        return State(0); // normall iteration, no ignorer detected
    };

    while ( *pc ) {
        if (find(&pair_index,  *pc,  up)) {
            pi_stack.push_back(pair_index);
        }
        else if (   pi_stack.size()
               &&   find(&pair_index,  *pc, down)
               && ( pair_index == pi_stack.back() ) )
        {
            pi_stack.pop_back();
        }
        if ( !pi_stack.size() && strchr(_seps, *pc) ) {
            // match found with a separator
            maybe_append_to_strs_from_str_substr();
            //start = pc+1;
            start=pc;
            next(start);
        }
        if ( next(pc) ) {
            return State(-6);
        }
    } // while *pc
    // last word preceeds  '\0' termination of _txt
    maybe_append_to_strs_from_str_substr();

    if (pi_stack.size()) {
        // Possibly unintended result, encapsulation is symetric chars are not matching
        return State(-4);
    }
    // succeeded
    return State(0);
}
    template <typename CONTAINER_T = std::vector<std::string> >
    state_t
Append_splitted( CONTAINER_T* strs_, std::string const& _str, char const* _seps, ASJ_special const& _asj )
{
    // std::cout<<HGREEN<<"Wrapped ASJ"<<NOCOLOR<<std::endl;
    return Append_splitted( strs_, _str, _seps, std::make_optional(_asj));
}
    template <typename CONTAINER_T = std::vector<std::string> >
    state_t
Append_splitted( CONTAINER_T* strs_, std::string const& _str, ASJ_special const& _asj )
{
    // std::cout<<HGREEN<<"2nd Wrapped ASJ"<<NOCOLOR<<std::endl;
    return Append_splitted( strs_, _str, ",", _asj);
}

//@} 3 overloads

//:Append_joined:// Inverse of Append_splitted
    template <typename CONTAINER_T=std::vector<std::string> >
    state_t
Append_joined(std::string * txt_, CONTAINER_T const& _strs,char const* _sepStr=" ")
{
    state_t return_value=State(0);
    if ( !_strs.size() ) {
        return State(1);     // no input to join, but result is valid because nothing would change,
        // however: no input possibly is not intended
    }
    // HERE: _strs.size >= 1
    if ( !txt_ ) {
        return State(-1);    // providing txt_ output pointer is mandatory
    }
    const size_t sep_size= strlen(_sepStr);
    if ( !sep_size ) {
        // Possibly unintended zero length separator, so strings are not separated
        return_value=State(2);
    }
    constexpr size_t RESEVERD_EXTRA=4; // relax boundaries, guarantees that reserved > needed
    size_t destr_start_len=txt_->size();
    size_t destr_len=RESEVERD_EXTRA+destr_start_len;
    for(auto str: _strs){
        destr_len+= sep_size; // for sep
        destr_len+= str.size();
    }
    txt_->reserve( destr_len);

    if ( destr_start_len>0 ) {
        // assume destr_ has 1 or more priviously joined items and separate following strings by _sep
        txt_->append( _sepStr);
    }

#if 0    // only works if there is a [] operator
    auto i_of_last_str= _strs.size() - 1;
    for ( decltype(i_of_last_str) i=0; i<i_of_last_str; ++i){
        txt_->append( _strs[i]);
        txt_->append( _sepStr);
    }
    // join last string, not followed by separator
    txt_->append(_strs[i_of_last_str]);
#else   // should work for most container types
    auto strs_size= _strs.size();
    decltype(strs_size) i=0;
    for(auto str: _strs){
        txt_->append( str);
        if ( ++i< strs_size ) {
            txt_->append( _sepStr);
        }
    }
#endif

    return return_value;
}
    template <typename CONTAINER_T=std::vector<std::string> >
    state_t
Append_joined(std::string * txt_, CONTAINER_T const& _strs,char _sepChr)
{
    std::string sepStr={_sepChr};
    return Append_joined(txt_,_strs,sepStr.c_str());
}
//@} to be moved to separate unit

    template<typename... _P> ASSUME_USED constexpr inline
    void
Fake_use(_P... /*_args*/ )
{
    //(void)(_args),...;
}

void Info_ProgramArguments();
state_t LeftShiftOut_First_ProgramArgument(std::vector<std::string>::size_type _number_of_additiional_parameters=0);
// void On_signal(int _signal);
void Initialize(int _argC, char const* _argV[]);
// For now: Allow to use old name.
#define ShowArgs Info_ProgramArguments

//:__TU_FS:// Single and short replacement, namespace alias bad if used in header. Local to declarations here.
#define __TU_FS std::filesystem
state_t FileInPATH( __TU_FS::path* canonical_path_, __TU_FS::path _filename, char const* _dirs=nullptr /* $PATH */);
state_t PathWriteTime(std::time_t *time_,__TU_FS::path _path);
#undef __TU_FS

//TODO: rename to DemangledTypeName
//:type_name()  gets the demanged type name of type T
    template<typename T>
std::string type_name()
{
    return boost::core::demangle( typeid(T).name());
}
//:type_name(object)  gets the demanged type name of object
    template<typename T>
std::string type_name(T const &)
{
    return boost::core::demangle( typeid(T).name());
}

// MT20180304 new
// The C++ system can not make types depending on the wordsize of the cpu for which it compiles.
// Normally you do want that behaviour so the types are (to a certain level) not depending on
// the cpu architecture (unless you make some common assumptions.
// Here types are defined that will tune into the wordsize of the cpu (in actual_cpuword)
// Probably determining the cpu word size (aka ACTUAL_CPUWORD_SIZE = 8 bits ... 64 bit) needs to be reviewed
// and improved.

constexpr int_fast16_t ACTUAL_CPUWORD_SIZE =  sizeof(unsigned char*) * CHAR_BIT;
using ACTUAL_CPUWORD_SIZE_type = decltype(ACTUAL_CPUWORD_SIZE);

template <decltype(ACTUAL_CPUWORD_SIZE) _Bits>
struct cpuword
: std::false_type  // unless specialized, no types provided _Bits
{
};

template <>
struct cpuword<8>
: std::true_type
{
    using signed_type = int8_t;
    using unsigned_type = uint8_t;
};

template <>
struct cpuword<16>
: std::true_type
{
    using signed_type = int16_t;
    using unsigned_type = uint16_t;
};

template <>
struct cpuword<32>
: std::true_type
{
    using signed_type = int32_t;
    using unsigned_type = uint32_t;
};

using actual_cpuword = cpuword<ACTUAL_CPUWORD_SIZE>;

template <>
struct cpuword<64>
: std::true_type
{
    using signed_type = int64_t;
    using unsigned_type = uint64_t;
};

template <>
struct cpuword<128>
: std::true_type
{
    using signed_type = int128_t;
    using unsigned_type = uint128_t;
};


/* A RAII_Counter can be used to monitor recursions or detect entering and leaving scope.
 It has many purposes, from managing stacks, detect object method recursions, signalling of enterering and leaving scopes.
*/
    template <typename Counter_T> class
    // Counter_T requires ++ and -- operators
RAII_Counter
{
    Counter_T& m_cntref;

  public:
    RAII_Counter(Counter_T* _cntptr)
    : m_cntref(*_cntptr)
    {
        ++m_cntref;
    }

    ~RAII_Counter()
    {
        --m_cntref;
    }

        Counter_T const&
    cntref()
        const
    {
        return m_cntref;
    }
};

//:Mask    return value that can act as mask for _bits bits
// example: Mask(8)==255
    inline uint64_t
Mask(uint8_t _bits)
{
    // for _bits>64 the result is Mask(64)
    return ( 1ull << static_cast<uint64_t>(_bits) ) - 1ull;
}

#if 1
//:Bits2str   of given value it writes #_bits to charBfr_ as '0'/'1' stream
//            charBfr_ must have at least #(_bits+1) characters allocated before
    template<typename Integer_T>
    char const*
Bits2str(char* charBfr_, decltype(sizeof(Integer_T)) bits, Integer_T _value)
{
    bits = std::min( bits, sizeof(Integer_T)* 8 ); // cut off bits to maximum size that fits (aka: 64 bits)
    for ( char *cursor= charBfr_+bits-1; cursor>=charBfr_; --cursor ) {
        *cursor = ( _value& 1 ) + '0';
        _value>>= 1;
    }
    charBfr_[bits]='\0';
    return charBfr_;
}
//            in simple and single thread scenario's the character buffer is provided within the routine
    template<typename Integer_T>
    char const*
Bits2str(decltype(sizeof(Integer_T)) const _bits, Integer_T _value)
{
    thread_local static char char_buffer[1+(sizeof(Integer_T)*8)];
    return Bits2str(char_buffer , _bits, _value);
}

    template<typename Integer_T>
    char const*
Bits2str(Integer_T _value)
{
    const size_t bits=sizeof(Integer_T)*8;
    thread_local static char char_buffer[bits+1];
    return Bits2str(char_buffer , bits, _value);
}

#endif


//TODO:
// bool newer(const char* _leftfile, const char* _rightfile, bool return_on_fail)  // same as in c++shx.cpp except it should use std::filesystem
//

//:Is_Little_Endian Is initialized during runtime startup and will be true if running on a little endian processor, othewhise false.
extern const bool Is_Little_Endian;

// Use via SIGNATURETUPLE_HASH_CODE
    template <typename... Args_P>
    std::pair<size_t,size_t>
_SignatureTuple_hash_codes(std::string _origin, Args_P... )
    {
        //std::cout<<"\n*********" << _origin<<"*********\n";
        using signature_tuple_t = std::tuple<typename std::decay<Args_P>::type... >;
        auto hashOf{[](std::string _s) { return   std::hash<std::string>{}(_s);}};
        return std::pair<size_t,size_t>{ hashOf(_origin), hashOf(type_name<signature_tuple_t>()) };
    }

//:SIGNATURETUPLE_HASH_CODES
// Creates hash codes for a list of types (the signature),
// the first hash is of the origin as string, the second from the demangled typename.
// The origin hash adds addtional differentiation to the type hash, for minimizing the chance of name hash clashes.
// To be used for (cpx) meta coding template signature specific specializations of functions and data structures
// These codes are to be used also at compiletime, but can be reproduced at runtime.
// Objective is to make signature specific (specialized) data queues,@compiletime declared, selected at runtime.
#define SIGNATURETUPLE_HASH_CODES(...) tosics::util::_SignatureTuple_hash_codes( \
    std::string(__PRETTY_FUNCTION__) + std::string(VSTRINGS(__VA_ARGS__) ),__VA_ARGS__ )


// Generic const cast wrapper functionality to reuse non-const function of method in const input and const output context

struct ConstReturn_OverloadTag{
};
    extern ConstReturn_OverloadTag
With_ConstReturn;

    // ----- version: lambda const pointer
    template< class C, typename R, typename L> R const&
const_wrap_callable( L call_expression, C const& c)
{
    return const_cast<R const&>(call_expression(const_cast<C&>(c)));
}
    // ----- version: lambda const reference
    template< class C, typename R, typename L> R const*
const_wrap_callable( L call_expression, C const* c)
{
    return const_cast<R const*>(call_expression(const_cast<C*>(c)));
}
    // ----- version: function const pointer
    template< class C,typename R,typename... A> R const*
const_wrap( C const* c, R* (*f)(C*,A&&...),A&&...a)
{
    auto call_expression=[&](C* cc)->R*
    {
        return (*f)(cc,std::forward<A>(a)...);
    };
    return const_wrap_callable< C, R, decltype(call_expression) >( call_expression, c);
}
    // ----- version: function const reference
    template< class C,typename R,typename... A> R const&
const_wrap( C const& c, R& (*f)(C&,A&&...),A&&...a)
{
    auto call_expression=[&](C& cc)->R&
    {
        return (*f)(cc,std::forward<A>(a)...);
    };
    return const_wrap_callable< C, R, decltype(call_expression) >( call_expression, c);
}
    // ----- version: method const pointer
    template< class C, typename R, typename... A> R const*
const_wrap( C const* c, R* (C::*f)(A&&...),A&&...a)
{
    auto call_expression=[&]( C* cc)->R*
    {
        return (cc->*f)(std::forward<A>(a)...);
    };
    return const_wrap_callable< C, R, decltype(call_expression) >( call_expression, c);
}
    // ----- version: method const reference
    template< class C, typename R, typename... A> R const&
const_wrap( C const& c, R& (C::*f)(A&&...),A&&...a)
{
    auto call_expression=[&]( C& cc)->R&
    {
        return (cc.*f)(std::forward<A>(a)...);
    };
    return const_wrap_callable< C, R, decltype(call_expression) >( call_expression, c);
}
    template<typename _TypeT>
    _TypeT*
ConstRemovedFrom(const _TypeT* _object)
{
        using
    nonconst_type = _TypeT*
    ;
        return
    const_cast<nonconst_type>(_object)
    ;
}
    template<typename _TypeT>
    const _TypeT*
ConstAddedFrom(_TypeT* _object)
{
        using
    const_type = const _TypeT*
    ;
        return
    const_cast<const_type>(_object)
    ;
}

    template<typename _TypeT>
    _TypeT&
ConstRemovedFrom(const _TypeT& _object)
{
        using
    nonconst_type = _TypeT&
    ;
        return
    const_cast<nonconst_type>(_object)
    ;
}
    template<typename _TypeT>
    const _TypeT&
ConstAddedFrom(_TypeT& _object)
{
        using
    const_type = const _TypeT&
    ;
        return
    const_cast<const_type>(_object)
    ;
}


/*
 In addition to const_wrap, to guarantee the origin can not be modified by modifying functions
 after wrapping const cast,
 duplicate can be used to duplicate the origin to the copy location, where the application
 can manage the storage of that copy. After the (potentionally) modifying function is called,
 the object on the copy location might be effected instead of the origin. When and if to use
 it is up to the application.
 Needless to say that this allows to use modifying methods of a class to get results from (otherwhise)
 modified origin. This is usefull for implementing dry run's (assuming polite programming and methods
 do not affect data outside the origin class instance).
 */

template< class C>
C const& duplicate(C* pcopy_, C const& _origin)
{
    ASSERT(pcopy_);
    pcopy_-> ~C();
    *pcopy_= _origin;
    return *pcopy_;
}




//:ByteAddress_cast
//Convert to pointer type that can be used to calculate byte offsets between them
template <
        typename _ClassT
    >
    std::byte *
ByteAddress_cast(_ClassT* _pClassInstance)
{
    return reinterpret_cast<std::byte*>( _pClassInstance);
}

template <
        typename _ClassT
    >
    constexpr std::byte const*
ByteAddress_cast(_ClassT const* _pClassInstance)
{
    return const_wrap( _pClassInstance, &tosics::util::ByteAddress_cast<_ClassT>);
}

//:RunAndFinally
// approximation of missing 'finally' in try catch independent fromn try catch
// Introducing $$$ prefix for visible implementation details that you should not use in user programs.
//
    template<typename _RunF, typename _FinalF>
    struct
RunAndFinally
;
    template<typename _RunF, typename _FinalF>
class $$$VoidRunAndFinally
{
    // everything is private, only the friend class(es) can use it
    friend class RunAndFinally<_RunF,_FinalF>;

    _FinalF m_final;

    $$$VoidRunAndFinally(_RunF _run, _FinalF _final)
    : m_final{_final}
    {
        try {
            _run();
        }
        catch(...) {
            m_final();
            throw;
        }
    }

    ~$$$VoidRunAndFinally()
    {
        m_final();
    }
}
;
    template<typename _RunF, typename _FinalF>
class $$$ReturnRunAndFinally
{
    // everything is private, only the friend class(es) can use it
    friend class RunAndFinally<_RunF,_FinalF>;

    _RunF m_run;
    using return_value_type = LAMBDA_RETURN_RESULT_TYPE(_RunF);
    _FinalF m_final;
    return_value_type return_value;

    $$$ReturnRunAndFinally(_RunF _run, _FinalF _final)
    : m_run{_run}, m_final{_final}
    {
        try {
            // in case of exception, can not run destructor because instance is not being constructed
            return_value = m_run();
        }
        catch(...) {
            // finally handling for incompleted object
            m_final();
            throw;
        }
    }
    ~$$$ReturnRunAndFinally()
    {
        m_final();
    }
}
;
    template<typename _RunF, typename _FinalF>
    struct
RunAndFinally
{
        using
    implementing_type =
        std::conditional_t< std::is_same_v<void,LAMBDA_RETURN_RESULT_TYPE(_RunF)>,
        /* then */ $$$VoidRunAndFinally<_RunF,_FinalF>,
        /* else */ $$$ReturnRunAndFinally<_RunF,_FinalF>
        >
    ;
  private:
        implementing_type
    m_implementor
    ;
  public:
    RunAndFinally(_RunF _run, _FinalF _final)
    : m_implementor{_run,_final}
    {
    }

    LAMBDA_RETURN_RESULT_TYPE(_RunF) return_value()
    {
if constexpr(std::is_same_v<void,LAMBDA_RETURN_RESULT_TYPE(_RunF)>){
       // this property: return_value(), exists also if the return result type is void, but it does nothing
        return;
}else {
        return m_implementor.return_value;
}
    }
}
;

#if NO_COMMENT
// usage

    int dums[]{1,2,3,4}; // immetate manipulated resource references
    int return_value=-1;

    RunAndFinally {decltype(std::declval<_RunF>()
        [&]{ // Run
           INFO(VARVALS(dums));
           return_value = 0;
        }, // And
        [&]{ // Finally
           ranges::for_each(dums,[](int& d_){ d_=0; });
           INFO(VARVALS(dums,return_value));
        }
    };
    // expressions here are executed if no thrown exception is in flight,
    // so at normal completion or a earlier return
    return return_value;

#endif


// Note: None of the directory class keep track of the current directory, only the directory
//  during construction. You can change the current directory (anyway) without "directory" objects
//   being aware. It requires manual action to keep the state of the the current directory.

// base class for duplicated code.
class DirectoryChanger
{
  public:
    class changeDir_error
    : public std::runtime_error
    {
      public:
        changeDir_error(const std::string& _what)
        : std::runtime_error(_what)
        {}
    };

    static state_t changeDir(const std::filesystem::path& _otherDir);
};

// keeps track of directory at the instantiation
class ScopedDirectory
: public DirectoryChanger
{
    std::filesystem::path m_before;

  public:
    ScopedDirectory(const std::filesystem::path& _otherDir)
    {
        if ( _otherDir.empty() ) {
            return;
        }
        m_before= std::filesystem::current_path();
        changeDir( _otherDir);
    }

    ScopedDirectory(ScopedDirectory&& _from)
    {
        m_before.clear(); // avoid restoring to this directory when instance destructor (if _from) gets called
        std::swap(_from.m_before,m_before);
    }

    ~ScopedDirectory()
    {
        changeDir(m_before);
    }

  private:
    ScopedDirectory(const ScopedDirectory&) = delete;
    ScopedDirectory& operator = (const ScopedDirectory&) = delete;
};

// more sophisticated directory stack, works like pushd and popd in bash
class DirectoryStack
: public DirectoryChanger, public std::vector<std::filesystem::path>
{
  public:
        void
    pushDir()
    {
        push_back( std::filesystem::current_path());
    }

    DirectoryStack()
    {
        pushDir(std::filesystem::current_path());
    }
        void
    popDir()
    {
        changeDir(back());
        pop_back();
    }

    void pushDir(const std::filesystem::path& _otherDir)
    {
        pushDir();
        changeDir( _otherDir);
    }

    ~DirectoryStack()
    {
        if ( empty() ) {
            // Everything was already popped, no more information left to act on.
            return;
        }
        // Go back to the directory when this instance was constructed.
        // We don't care managing the stack since it all distrcuted anyway.
        changeDir(front());
    }
};

// repeat: avoid the nastyness of std::ranges::iota_view .
    inline auto
repeat(unsigned long _offset, unsigned long _count)
{
    return std::ranges::iota_view(_offset,static_cast<unsigned long>(_count));
}

//:repeats Do _count iterations, no need to give the range, it is mostly 0.._count-1 anyway.
    inline auto
repeat(unsigned long _count)
{
    return repeat(static_cast<unsigned long>(0),static_cast<unsigned long>(_count));
}

//:FAKE_USE
/// compiler portable suppress "unused variable" warning
#define FAKE_USE(...) tosics::util::Fake_use(__VA_ARGS__)

// __builtin_expect (long expression, long constant) expects that
#define EXPECT(_expression,_value) (__builtin_expect (static_cast<long>(_expression),static_cast<long>(_value)))
// else
//#define EXPECT(_expression,_value) (_expression)
//..(static_cast<long>(_expression)==static_cast<long>(_value))
// endif gcc compiler

#define EXPECT_true_FROM(_expression) EXPECT(_expression,true)
#define EXPECT_false_FROM(_expression) EXPECT(_expression,false)


//:Cstr2uint64    Convert 'at compile time' ASCII characters to a 64 bit integer.
   inline constexpr uint64_t
charIdCode(char c)
{
   auto index = static_cast<uint64_t>(static_cast<unsigned char>(c));
   // index >= 32  when <32 is harmless but is undefined behavour and could cause collisions.
   return index- 32ull;
}

    inline constexpr
    uint8_t
MostFrequntUsedEncode(uint8_t _c)
{
    // Optimized (in creating hashes that do not collide) for
    // command words, they may contain _ - a..z A..Z 0..9
    // other values (tokens) do not contribute to the hash.
    // See whtablegen.php
        constexpr uint8_t
    COMMAND_ENCODING_TABLE[256]=
    {   0, 255, 254, 253, 252, 251, 250, 249, 248,  96,  97, 247, 246,  97, 245, 244
    , 243, 242, 241, 240, 239, 238, 237, 236, 235, 234, 233, 232, 231, 230, 229, 228
    ,  95,  69,  89,  66,  64,  78,  75,  90,  82,  83,  73,  72,  92,  71,  67,  74
    ,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  68,  91,  86,  88,  87,  70
    ,  65,  29,  46,  39,  36,  27,  41,  43,  35,  31,  49,  48,  37,  40,  32,  30
    ,  45,  50,  34,  33,  28,  38,  47,  42,  52,  44,  51,  84,  93,  85,  79,  63
    ,  94,   4,  20,  13,  10,   1,  15,  17,   9,   5,  23,  22,  12,  14,   6,   3
    ,  19,  24,   8,   7,   2,  11,  21,  16,  26,  18,  25,  80,  77,  81,  76,  99
    , 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115
    , 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131
    , 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147
    , 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163
    , 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179
    , 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195
    , 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211
    , 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227
    };
        auto
    index=_c
    ;
    return COMMAND_ENCODING_TABLE[ index];
}
    inline constexpr
    uint8_t
MostFrequntUsedEncode(char _c)
{
    return MostFrequntUsedEncode(static_cast<uint8_t>(_c));
}



//:Cstr2uint64    Convert 'at compile time' ASCII characters to a 64 bit integer hash.
//
//  The aim for this function is to facilitate switch/case-s with string literals rather then numbers.
//  This avoids ugly if else chains.
//
// Words that are shorter then 10 characters, nearly guaranteed unique. Intended for (DSL) language keywords or
// ((long)commandline) options. All used numbers are prime numbers, they are used to cause maximum irregilarity
// to to maximize chance on unique output from any input. You can use 1>>58 i.s.o devision amd 1<<7 for multiplication,
// but with less uniquenes guarantee. For the best results, character codes should be in range 32..127. <32 is
// undefined. Check s before when needed. The function stress tested on real world data and has no known collisions.
// Due to the small loop, the loop should be completely executed from the code cache.
    inline constexpr  uint64_t
Cstr2uint64(const char* cs, size_t len)
    noexcept
{
    if ( !len ) return 0ull;
    ASSERT(!cs[len]);

        constexpr uint64_t
    CHAR_FACTOR=97
    ;
        uint64_t
    value_feedback_divider= 190172619316593311ull; // max_prime_below((2^64)/CHAR_FACTOR)
    ;
        uint64_t
    out=len
    ;
    if  (
    #define TU_FIRST_CHARS ( ((out*=CHAR_FACTOR)+=MostFrequntUsedEncode(*cs++)), *cs)
            TU_FIRST_CHARS
         && TU_FIRST_CHARS
         && TU_FIRST_CHARS
         && TU_FIRST_CHARS
         && TU_FIRST_CHARS
         && TU_FIRST_CHARS
         && TU_FIRST_CHARS
         && TU_FIRST_CHARS
         && TU_FIRST_CHARS
     #undef FIRST_CHARS_DONE
        ) {
        do {
           // cycled is saving upper bits by rotating, before they get lost in next multiplication.
            uint64_t cycled= out/ value_feedback_divider;
            ( ( out*= CHAR_FACTOR )+= cycled )+= MostFrequntUsedEncode( *cs++ );
        } while (*cs);
    }

    return out;
}

    inline constexpr  uint64_t
Cstr2uint64v(const char* cs, size_t len)
    noexcept
{
    ASSERT(!cs[len]);
    if ( !len ) return 0ull;

    constexpr uint64_t
    CHAR_FACTOR=97
    ;
    constexpr uint64_t
    value_feedback_divider= 190172619316593311ull; // max_prime_below((2^64)/CHAR_FACTOR)
    ;
    uint64_t
    out=len
    ;

    for (   const char* cs_end=cs+9;
            ( *cs )&& ( cs< cs_end );
            ( out*=CHAR_FACTOR )+= MostFrequntUsedEncode(*cs++)
        );

    while ( *cs ) {
        // cycled is saving upper bits  by rotating, before they get lost in next multiplication.
        uint64_t cycled= out/ value_feedback_divider;
        ( ( out*= CHAR_FACTOR )+= cycled )+= MostFrequntUsedEncode( *cs++ );
    }

    return out;
}


    inline constexpr uint64_t
Cstr2uint64(char const* const _cs)
    noexcept
{
    return Cstr2uint64( _cs, strlen(_cs));
}
    inline
    uint64_t /*runtime*/
Cstr2uint64(std::string const& _s)
    noexcept
{
    return Cstr2uint64( _s.c_str(),_s.size());
}

// S_tring L_iteral H_ash
//  switch ( Cstr2uint64(itemString) { case "firstcase"_slh: .... break; case "second"_slh: .....}
//  switch ( Cstr2uint64(itemString) { case SLH(firstcase):.......break; case SLH(second):.....
    inline constexpr uint64_t
operator "" _slh(const char* _s,size_t _l)
    noexcept
{
    return Cstr2uint64( _s, _l);
}

#define SLH(word) Cstr2uint64(#word)
// consistant with S128LH
#define S64LH(word) SLH(word)

//_____________________________________________________________________________________________________________________

//:Cstr2uint128    Convert 'at compile time' ASCII characters to a 128 bit integer. Identical to Cstr2uint64 but more unique.
// Words that are shorter then 10 characters, nearly guaranteed unique
    inline constexpr uint128_t
Cstr2uint128(const char* cs, size_t len)
    noexcept
{
    if ( !len ) return 0ull;
    ASSERT(!cs[len]);

        constexpr uint64_t
    CHAR_FACTOR=97
    ;
        uint128_t
    //value_feedback_divider= 3508065638360190344983243375585239263ullll; // max_prime_below((2^128)/CHAR_FACTOR)
    //direct 128 digits input needs to be split into upper and lower 64 bit values
    value_feedback_divider=(uint128_t(190172619316593315ull)<<64) | uint128_t(11600529778312192223ull);
    ;
            uint128_t
    out=len
    ;
    if  (
        #define TU_FIRST_CHARS ( ((out*=CHAR_FACTOR)+=MostFrequntUsedEncode(*cs++)), *cs)
            TU_FIRST_CHARS
         && TU_FIRST_CHARS
         && TU_FIRST_CHARS
         && TU_FIRST_CHARS
         && TU_FIRST_CHARS
         && TU_FIRST_CHARS
         && TU_FIRST_CHARS
         && TU_FIRST_CHARS
         && TU_FIRST_CHARS
        //first 9 (in first 64 bit)
         && TU_FIRST_CHARS
         && TU_FIRST_CHARS
         && TU_FIRST_CHARS
         && TU_FIRST_CHARS
         && TU_FIRST_CHARS
         && TU_FIRST_CHARS
         && TU_FIRST_CHARS
         && TU_FIRST_CHARS
         && TU_FIRST_CHARS
        //first 18 (in 128 bit)
        #undef FIRST_CHARS_DONE
        ) {
        do {
           // cycled is saving upper bits by rotating, before the get lost in next multiplication.
            uint128_t cycled= out/ value_feedback_divider;
            ( ( out*= CHAR_FACTOR )+= cycled )+= MostFrequntUsedEncode( *cs++ );
        } while (*cs);
    }

    return out;
}

    inline constexpr uint128_t
Cstr2uint128(char const* const _cs)
    noexcept
{
    return Cstr2uint128( _cs, strlen(_cs));
}
    inline uint128_t /*runtime*/
Cstr2uint128(std::string const& _s)
    noexcept
{
    return Cstr2uint128( _s.c_str(),_s.size());
}

// S_tring 128 bit long L_iteral H_ash
//  switch ( Cstr2uint128(itemString) { case "firstcase"_slh: .... break; case "second"_slh: .....}
//  switch ( Cstr2uint128(itemString) { case SLH(firstcase):.......break; case SLH(second):.....
    inline constexpr uint128_t
operator "" _s128lh(const char* _s,size_t _l)
    noexcept
{
    return Cstr2uint128( _s, _l);
}
#define S128LH(word) Cstr2uint128(#word)

//_____________________________________________________________________________________________________________________

    inline constexpr
    uint8_t
MostFrequntUsedDecode(uint8_t _encoded)
{
        constexpr
        uint8_t
    COMMAND_DECODING_TABLE[256]=
    {   0, 101, 116, 111,  97, 105, 110, 115, 114, 104, 100, 117, 108,  99, 109, 102
    , 119, 103, 121, 112,  98, 118, 107, 106, 113, 122, 120,  69,  84,  65,  79,  73
    ,  78,  83,  82,  72,  68,  76,  85,  67,  77,  70,  87,  71,  89,  80,  66,  86
    ,  75,  74,  81,  90,  88,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  95
    ,  36,  64,  35,  46,  58,  33,  63,  45,  43,  42,  47,  38, 126, 124,  37,  94
    , 123, 125,  40,  41,  91,  93,  60,  62,  61,  34,  39,  59,  44,  92,  96,  32
    ,   9,  13,   0, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139
    , 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155
    , 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171
    , 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187
    , 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203
    , 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219
    , 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235
    , 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251
    , 252, 253, 254, 255,  31,  30,  29,  28,  27,  26,  25,  24,  23,  22,  21,  20
    ,  19,  18,  17,  16,  15,  14,  12,  11,   8,   7,   6,   5,   4,   3,   2,   1
    };

    auto index=  _encoded;
    auto decoded=  COMMAND_DECODING_TABLE[ index];
    return decoded;
}
    inline constexpr
    uint8_t
MostFrequntUsedDecode(char _c)
{
    return MostFrequntUsedDecode(static_cast<uint8_t>(_c));
}
    template<typename HASH_T>
    inline constexpr
    state_t  // Investigate returned state using STATEREPORT
hashToSmallCstr( uint8_t* cstr_, HASH_T hash)
{
    if ( !cstr_ ) {
        return state_t{-1}; // oops: nullptr, avoid writing to it, caller needs to reserve at least len+1 characters pointed by cstr_. uint8_t[20] is sufficient.
    }
    *cstr_='\0';
    if ( !hash ) {
        return state_t{0};  // hash of an empty string
    }
    // Determine length of the string to extract and cache the divisions
        size_t
    len=0
    ;
        constexpr size_t
    MaxLen= ( sizeof(HASH_T)== sizeof(uint64_t) )? 9: 18;
    ;
        HASH_T
    cache[MaxLen+2]  // avoid doing divisions twice
    ;
    while( hash!= len ) {
        cache[ len]= hash;
        hash/= 97;
        if ( !hash ) {
            return state_t{-2};  // No matching hash with len found, possibly earlier fragments are cycled and the value is folded, decoding not possible.
        }
        ++len;

        if (len> MaxLen) {
            return state_t{-3};  // Hash exceeds the maximum length, earlier fragments are cycled and the value is folded, decoding not possible.
        }
    }
    // Cache cleaning is not strictly neccesary but it contributes to application robustness by clearing uninitialized values.
    cache[ len]= hash;
        size_t
    ci=len
    ;
    while ( ++ci< ( MaxLen+ 2 ) ) {
        cache[ci]=static_cast<HASH_T>(0);
    }
    // Extract the string characters from end of the string to the beginning of the string
    ci=0;
        HASH_T
    last
    ;
    cstr_[ len]='\0';
    do {
        --len;
        last= cache[ ci++]% 97;
        if ( !last ) {
            return state_t{-4}; // too early zero termination
        }
        auto encoded= static_cast<uint8_t>(last);
        auto decoded= MostFrequntUsedDecode( encoded);
        cstr_[ len]= decoded;
    } while (len);

    return state_t{0}; // Success
}
            /*** INSERT NEW CODE ABOVE ***/


} //namespace tosics::util

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/**********************************************************************************************************************/
/*************************************** BROKEN OR DEPRICATED *********************************************************/
/**********************************************************************************************************************/
// Despite of being broken or deprecated, some remain in the library because they where used elsewhere and/or
// state certain concepts that remain alive.
//

/** \def Properties

New concept for properties

Use fixed name conventions and decltype to extract the type from the name.

class classname has property propertyname
  Then the storing membername is m_propertyname
  The getter declaration is decltype(m_propertyname) propertyname() const
  The setter declaration is void propertyname(const decltype(m_propertyname)& _propertyname)  and

 There is no implementation definition (it is unwhise to make one, it surpasses the purpose of properties
  to keep all invariants of the instance consistent. In the extreme case there is not even a implementation
  storage for the property but is the property constructed from other instance members).

 But a bare getter only does
  return m_propertyname
 And a bare setter only does
  m_propertyname = _propertyname.

  The following should be valid:
    objectname.propertyname(objectname.propertyname());        // set object property to it self
  So the getter returntype is the same as the setter input type.
  Even if there is no decltype(objectname)::m_propertyname attribute, the methods are still considered property
  setter and getter.

  Definition:

    A setter is a method that has a single input argument as its signature that will not be modified,
    but the setter will modify the object up on which it is called. Multiple calls of a setter with the
    same input value do not change the object again.  If it does, it is not a setter.

    A getter is a method without a argument in its signature but returning a value (intended) type without
    modification of the object uppon wich it is called. Multiple calls of a getter will result in the same
    output value. If it does, it is not a getter.

    Deprecated and obsolete
     get_propertname(), set_propertyname(arg). Use propertyname() and propertyname(arg).

    Recomendation
      Prefere to write methods as properties. But distinguish other methods (if they have no/single arg)
      by clear descriptive naming.

*/
/// @brief  Give the signature for object data input by a setter with name: propertyname
// BROKEN: should not asume existance of m_##propertyname. But stick to this signature for a setter.
//         decalaration cannot be generalized, it differs per class and per property.
//         So far (2017-05) only preserve depended on it, so those property methods are declared natively instead.
// #define DECLARE_SETTER(propertyname) void propertyname(const decltype(m_##propertyname)& _##propertyname)

/// @brief  Give the signature for object data output by a getter with name: propertyname
// BROKEN: See ex[planation at DECLARE_SETTER
// #define DECLARE_GETTER(propertyname) decltype(m_##propertyname) propertyname()


// if def compiler is gcc  (so far, always)

#endif //UTILS_HPP_
