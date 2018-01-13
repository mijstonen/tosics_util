#pragma once
//util.hpp
////////////////////
#define BUILD_ALL 1
// BUILD_ALL comment. BUILD_ALL must be at the first line so it can be changed very easily by a automated tool.
// Touch , to force a rebuild all without make clean and also force ccache not to shortcut any way, make changes to
// the BUILD_ALL macro. There is and should be no further use of it.The value does not matter,
// as long it changes, increase it by one each time you request a complete rebuild makes sense.

// #include <stdinc.hpp> has to be precompiled and passed the compiler with -include

#ifndef UTILS_HPP_
#define UTILS_HPP_ 1


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

 If the assertion failes, the (failed) action is reported
 usage example: ASSERT_ALWAYS_DO(initialize_program())
*/
# define ASSERT_ALWAYS_DO(_action) \
  ((_action) \
   ? __ASSERT_VOID_CAST (0) \
   : __assert_fail (__STRING(_action), __FILE__, __LINE__, __ASSERT_FUNCTION))

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

#endif  // DEBUG

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
namespace tosics::util {

    // NOTE: Deliberately restricted to (most usefull) foreground coloring.
    //       There are several ANSI console codes libraries for more sophisticated features.
    constexpr char const* NOCOLOR = "\033[00m";

    constexpr char const* RED = "\033[00;31m";
    constexpr char const* YELLOW = "\033[00;33m";
    constexpr char const* GREEN = "\033[00;32m";
    constexpr char const* CYAN = "\033[00;36m";
    constexpr char const* BLUE = "\033[00;34m";
    constexpr char const* MAGENTA = "\033[00;35m";
    constexpr char const* WHITE = "\033[00;37m";

    constexpr char const* HRED = "\033[01;31m";
    constexpr char const* HYELLOW = "\033[01;33m";
    constexpr char const* HGREEN = "\033[01;32m";
    constexpr char const* HCYAN = "\033[01;36m";
    constexpr char const* HBLUE = "\033[01;34m";
    constexpr char const* HMAGENTA = "\033[01;35m";
    constexpr char const* HWHITE = "\033[01;37m";

//{ SCX   stands for System Const eXPression.
// Encapsulate system constants that trigger old-style-cast warnings.
// They are allowed here because we do not alter system files
// but no where else. THe name should be SCX_<orginal name>
//
//:SCX_MAP_FAILED
/// Same as MAP_FAILED but avoid old-style-cast warnings of the compiler, instantiated in util.cpp
extern const void* SCX_MAP_FAILED;
//} SCX



// MT20170417, Some forward declarations/definitions, see statereport.hpp
//  defined state_t and stateLiteralArg_t are new and might not be used everywhere, using long and int should work well

//:state_t:// The CPU natuaral integer (can vary per processor, only use small(..est possible) values)
using state_t = long;

//:stateLiteralArg_t:// not passing a integer literal should be detected by the compiler (by using move semantics)
using stateLiteralArg_t = state_t&&;

//:State:// The intend is to make searches easy where a state is thrown or returned. Aka State(0) State(-1) State(12)
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
    align_size_to_multi_min( 475, 16 ); // (29+1)*16 = 480  480>475

    align_size_to_multi_min(43,12);     // non power 2 alignment returns 48

    align_size_to_multi_min(1333,1024)  // returns 2048

    new_map_size= align_size_to_multi_min( new_map_size, mh.page_size); // make new_map_size aligned
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
    return static_cast<decltype(_toInstance)> ( _fromInstance );
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
 @param[in,out] _NumberOfItems_in_indexOut_ in: Number of items in the array (see below),
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
FindIndex( int *_NumberOfItems_in_indexOut_, const char* _texts[])
{
    ASSERT( ( *_NumberOfItems_in_indexOut_ )> 0 );  // item to be found has to be on _texts[0]

    const char* item_to_be_found= _texts[0];
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wempty-body"  // for clang++
    while  ( strcmp( _texts[--( *_NumberOfItems_in_indexOut_ )], item_to_be_found ) ){
        // DBG_INFO(VARVAL(_NumberOfItems_in_indexOut_)); FAKE_USE(_NumberOfItems_in_indexOut_);
    };
#pragma GCC diagnostic pop

    ASSERT( ( *_NumberOfItems_in_indexOut_ )>= 0 );
}

//
// Would this be faster ? (by forward pre scanning by the processor)
//
    inline void
ForwardFindIndex( int *_NumberOfItems_in_indexOut_, const char* _texts[])
{
    ASSERT( ( *_NumberOfItems_in_indexOut_ )> 0 );// item to be found has to be on _texts[_NumberOfItems_in_indexOut_-1]
    const char* item_to_be_found= _texts[ ( *_NumberOfItems_in_indexOut_ )- 1 ];
    ( *_NumberOfItems_in_indexOut_ )=0;
    while  ( strcmp( _texts[( *_NumberOfItems_in_indexOut_ )++], item_to_be_found ) ){
        // DBG_INFO(VARVAL(_NumberOfItems_in_indexOut_)); FAKE_USE(_NumberOfItems_in_indexOut_);
    };
}

char const* DateTime();

    void
Info_main_args (int argc, char const* argv[]);

#define SHOW_ARGS info_main_args(argc,argv)

#define MAX_BT_BUFS 100

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

void DumpBacktraceInFileStream(int backtrace_output_filehandle_ = STDERR_FILENO);


//@{ to be moved to separate unit
//:ThrowBreak:// Use it i.s.o. naked throw and put breakpoints in it and see full backtrace in debugger
    template <typename EXCEPTION_T>
    [[ noreturn ]] void
ThrowBreak(EXCEPTION_T _exception, eBreakCategory _break_category = eBC_default)
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
            throwBreak_EBC(eBC_ui);                      // these lines
            throwBreak_EBC(eBC_service);                 //
            throwBreak_EBC(eBC_aync);
            throwBreak_EBC(eBC_assertion_failed);
            throwBreak_EBC(eBC_handled);
            throwBreak_EBC(eBC_fatal);
        }
#undef throwBreak_EBC
        std::cerr<< std::endl<< HRED << __PRETTY_FUNCTION__
                 << " break_category_name: '"<< break_category_name
                 << "'    dumping stacktrace!"
                 << NOCOLOR<<std::endl
                 ;
        DumpBacktraceInFileStream(STDERR_FILENO);
    }
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

//:Append_splitted:// For VARVALS implementation, to split string with arguments
//@{ 3 overloads
    template <typename CONTAINER_T = std::vector<std::string> >
    state_t
Append_splitted( CONTAINER_T* strs_, std::string const& _str
                       , char const* _seps = ","
#if 1  // 0 for clang workarround
                       , std::experimental::optional<ASJ_special> const& _optAsj=
                                                    std::experimental::optional<ASJ_special>()
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
    return Append_splitted( strs_, _str, _seps, std::experimental::make_optional(_asj));
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

    template<typename Last_T>
    inline void
fake_use( Last_T const& /*_last*/)
{
    //(void)( _last);
}
    template<typename First_T, typename... Remaining_T>
    inline void
fake_use( First_T const& _first, Remaining_T const&... _remaining)
{
    fake_use( _first);
    fake_use( _remaining...);
}

extern std::vector<std::string> ProgramArguments;
void Info_ProgramArguments();
void On_signal(int _signal);
void Initialize(int _argC, char const* _argV[]);
// For now: Allow to use old name.
#define ShowArgs info_ProgramArguments

//:__TU_FS:// Single and short replacement, namespace alias bad if used in header. Local to declarations here.
#define __TU_FS std::experimental::filesystem
state_t FileInPATH( __TU_FS::path* canonical_path_, __TU_FS::path _filename, char const* _dirs=nullptr /* $PATH */);
state_t PathWriteTime(std::time_t *time_,__TU_FS::path _path);
#undef __TU_FS


//TODO:
// bool newer(const char* _leftfile, const char* _rightfile, bool return_on_fail)  // same as in c++shx.cpp except it should use std::experimental::filesystem
//

} //namespace tosics::util

//:FAKE_USE
/// compiler portable suppress "unused variable" warning
#define FAKE_USE(...) tosics::util::fake_use(__VA_ARGS__)


/**********************************************************************************************************************/
/*************************************** BROKEN OR DEPRICATED *********************************************************/
/**********************************************************************************************************************/
// Despite of bein broken or deprecated, some remain in the library because they where used elsewhere and/or
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

//:Throw_Error_Break
/**
@deprecated
Use template <typename EXCEPTION_T>
void throwBreak(EXCEPTION_T _exception, eBreakCategory _break_category = eBC_default) instead.

@brief drop in replacement for a bare 'throw' expression to get a stack trace in a debugger during the exception

 @tparam[in] _e pass the exception of any Exception_T type

 Normally a exception hides the stack trace because it is unrolled the program is left at the catch handler.
 Putting a breakpoint on the catch handler is not (so) very usefull. But by wrapping each throw expression
 by this template function, you can use a debugger to display the stacktrace during the exception.

 */
    template <typename Exception_T>
    void
Throw_Error_Break(const Exception_T& _e)
{
/*put a breakpoint here to have the stack trace in the debugger*/ throw _e; // pass the exception to some other handler
// NOTE: When instrumenting source code (SeqIntra) ensure instrumenation of this method.
}


#endif //UTILS_HPP_
