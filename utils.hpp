////////////////////
#define BUILD_ALL 1
// BUILD_ALL comment. BUILD_ALL must be at the first line so it can be changed very easily by a automated tool.
// Touch , to force a rebuild all without make clean and also force ccache not to shortcut any way, make changes to
// the BUILD_ALL macro. There is and should be no further use of it.The value does not matter,
// as long it changes, increase it by one each time you request a complete rebuild makes sense.


/// utils.h ///
#ifndef UTILS_HPP_
#define UTILS_HPP_ 1

#include "stdinc.hpp"

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

//:SR_ENABLE
//:SR_DEBUG
/**
 \def SR_ENABLE
 @brief enable (1) or disable (0) STATEREPORT
 @sa STATEREPORT

 When STATEREPORT is disabled it will not report unexpected values, it will pass the return value of the call unchanged.

 @remark With disabling, also the advantages of STATEREPORT is lost. It's your responsibillity to decide what is more
         importand, except that the customer could in rare cases encounter a message from STATEREPORT or be silent
         about unexpected states.
         Personally I would keep STATEREPORT always active.

 \def SR_DEBUG
 @brief Show source file and line number with the reported state or not.
 @sa STATEREPORT

 The name of the function or method will alwas be shown during the reported state.
*/


//#define DEBUG 0

// If FAST==1 it is like driving without seatbelt. Some redundant code that improves robustness is disabled
// to give you the best possible raw speed at higher risc of some instabillity, due to misperception of assumptions
// during the design (of cause, I do NOT do such thing, ha ha).
// #define FAST  1

//:CHAR2UINT_CAST
/// always convert ASCI character nummerics to range 0..255
#define CHAR2UINT_CAST(_CHR) static_cast<unsigned int>(static_cast<unsigned char>(_CHR))



//
// New concept for properties
//
// Use fixed name conventions and decltype to extract the type from the name.
//
// class classname has property propertyname
//   Then the storing membername is m_propertyname
//   The getter declaration is decltype(m_propertyname) propertyname() const
//   The setter declaration is void propertyname(const decltype(m_propertyname)& _propertyname)  and
//
//  There is no implementation definition (it is unwhise to make one, it surpasses the purpose of properties
//   to keep all invariants of the instance consistent. In the extreme case there is not even a implementation
//   storage for the property but is the property constructed from other instance members).
//
//  But a bare getter only does
//   return m_propertyname
//  And a bare setter only does
//   m_propertyname = _propertyname.
//

#define DECLARE_SETTER(propertyname) void propertyname(const decltype(m_##propertyname)& _##propertyname)
#define DECLARE_GETTER(propertyname) decltype(m_##propertyname) propertyname()



//:util
/**
 @brief common utillity operations widely used in the source and often practical for any project

 These operations evolved over the years, not all of it might be usefull the specific project.
 util moves from project to project and more functionallity is gathered.
*/
namespace util {
//
// Encapsulate system constants that trigger old-style-cast warnings.
// They are allowed here because we do not alter system files
// but no where else. THe name should be SCX_<orginal name>
// SCX stands for System Const eXPression.
//
# pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"


//:SCX_MAP_FAILED
/// Same as MAP_FAILED but avoid old-style-cast warnings of the compiler
constexpr const void* SCX_MAP_FAILED= MAP_FAILED;
# pragma GCC diagnostic pop

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

/// get the first addres beyond the last possible item with given size, used in macro END_OF
#define END_ADR(_C_array,_size)                          (_C_array+_size)

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


//:Throw_Error_Break
/** @brief drop in replacement for a bare 'throw' expression to get a stack trace in a debugger during the exception

 @tparam[in] _e pass the exception of any Exception_T type

 Normally a exception hides the stack trace because it is unrolled the program is left at the catch handler.
 Putting a breakpoint on the catch handler is not (so) very usefull. But by wrapping each throw expression
 by this template function, you can use a debugger to display the stacktrace during the exception.

 */
template <typename Exception_T>
void Throw_Error_Break(const Exception_T& _e)
{
/*put a breakpoint here to have the stack trace in the debugger*/ throw _e; // pass the exception to some other handler
// NOTE: When instrumenting source code (SeqIntra) ensure instrumenation of this method.
}




//:BYTE_MASK
/// mask 8 bits
#define BYTE_MASK  0xFF


//:NIBLE_MASK
/// mask lower 4 bits
#define NIBLE_MASK 0x0F


//:BIT_IN_BYTE_MASK
/// mask lowest 3 bits
#define BIT_IN_BYTE_MASK 0x07


//:PAIR_MASK
/// mask lower 2 bits
#define PAIR_MASK  0x03


//:BIT_MASK
/// mask lowest single bit
#define BIT_MASK   0x01


//:HIGH_BYTES_MASK
/// mask all except lower 8 bits
#define  HIGH_BYTES_MASK  (~BYTE_MASK)


//:HIGH_NIBLE_MASK
/// mask all except lower 4 bits
#define  HIGH_NIBLE_MASK (~NIBLE_MASK)


//:BITS_OUTSIDE_BYTE_MASK
///mask all except lower 3 bits
#define BITS_OUTSIDE_BYTE_MASK (~BIT_IN_BYTE_MASK)


//:HIGH_PAIRS_MASK
/// mask all except lower 2 bits
#define  HIGH_PAIRS_MASK (~PAIR_MASK)


//:HIGH_BITS_MASK
/// mask all except lower 1 bit
#define   HIGH_BITS_MASK   (~BIT_MASK)


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

//:FAKE_USE
/// compiler portable suppress "unused variable" warning
#define FAKE_USE(_v) ((void)(_v))

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

//______________________________________________________________________________________________________________________
//
// property setter and getters
// ---------------------------
// Asume standard property
// method names   void    classname::set_property('const' type '&/&&' _property)  and
//             '?'type'?' classname::get_property() const
// The in 'quotes' parts indicate that the macro's do not require a certain
// precise type declaration.
//
// _SET has prefix underscore to indicate that it is input to the object
// GET_ has postfix underscore to indicate output from the object
//
// The unique form of these macro's with _ and _FROM_ should reduce the chance of name clashses with other frameworks
// (macro's 'SET' and 'GET' would be too general). Besides the underscores and _FROM_ make the source more readable
// However there is no limitation for which classes


//:_SET
/// object._SET(property)_FROM_(value);
#define _SET(property) set_##property(


//:_FROM
/// object._SET(property)_FROM_(value);
#define _FROM_(...) __VA_ARGS__)


//:_PROPERTY_SET
/// ismpl work arround  for _SET(property)_FROM_(...)
#define _PROPERTY_SET(property,...) set_##property(__VA_ARGS__)


//:GET_
/// object.GET_(value)
#define GET_(property) get_##property()
//
// end property setter and getters
//______________________________________________________________________________________________________________________



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

size_t align_size_to_multi_min( size_t _size_, size_t const& _min);


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
 */
template<typename to_T, typename from_T>
inline
to_T static_to_type_cast(to_T _toInstance, from_T _fromInstance)
{
    return static_cast<decltype(_toInstance)>( _fromInstance);
}


//:Error_close
/**
 @brief Close the file while reporting a optional error message and a system error message
 @param[in] _fd The filedescriptor to close.
 @return -1 on error otherwhise 0
 @sa int Error_close( int _fd, std::string _perrormsg )
*/
int Error_close( int _fd);
/**
 @brief Close the file while reporting a optional error message and a system error message
 @param[in] _fd The filedescriptor to close.
 @param[in] _perrormsg  (optional) extra error message
 @return -1 on error otherwhise 0
 @sa int Error_close( int _fd)
*/
inline
int Error_close( int _fd, std::string _perrormsg )
{
    perror( _perrormsg.c_str());
    return Error_close( _fd);
}


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
int setFileSize( int _fd, size_t _size, bool _grow=true);



//: findIndex
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

  1. create a array kind of list of items
  2. put the item to search for as first  (items[0])
  3  Declare a int and initialize from from ITEMS_IN(items)
  4  Call findIndex
  5  Use the result, probably in a switch


  use   When decision is to be made on a limited (probably statically defined) number of strings,
        aka (small) (programing) language parsing.

 @sa util_demos.cpp:findIndex_demo()
*/

    inline void
findIndex( int *_NumberOfItems_in_indexOut_, const char* _texts[])
{
    ASSERT( ( *_NumberOfItems_in_indexOut_ )> 0 );  // item to be found hast to be on _texts[0]

    register auto item_to_be_found= _texts[0];
    while  ( strcmp( _texts[--( *_NumberOfItems_in_indexOut_ )], item_to_be_found ) );

    ASSERT( ( *_NumberOfItems_in_indexOut_ )>= 0 );
}



} //namespace util
#endif //UTILS_HPP_
