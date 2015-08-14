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

//______________________________________________________________________________________________________________________
//{
//:STATEREPORT
/**
 \def STATEREPORT

 @brief debugging aid to have a standard way to report any uncontrolled and unexpected return state of a low(er)
        level called function.

  STATEREPORT(  _CALL  {,exclusions} {,int/char _what})

 @param [in] _CALL : the wrapped method or function call

 @param [in] exclusions (optional) list of return states, this is C++11 specific
                        @e initialization-list syntax
                        If the exclusions list is ommitted then the default of {0} (is: SR_ZERO_IS_SUCCESS)
                        is used. Meaning, if the _CALL returned 0 it has completed successfully. When this is the case
                        and other values should be excluded ( e.g 1,-3 ) then 0 has to be included ( e.g {0,1,-3}
                        is SR_EXCLUDE_ALL(0,1,-3) is SR_EXCLUDE_0_AND(1,-3) )

 @param [in] _what  (optional) choose what to report, in all cases; except what is in the exclusions list
  - 0 'q' 'Q'  report nothing
  - 1 'n' 'N'  only report notifications (positive return statements)
  - 2 'e' 'E'  only report errors (negative reurn statements)
  - 3 'a' 'A'  report all, note that this is used rarely explicitly because if what is ommited then 3 is assumed

 @sa SR_EXCLUDE_ALL SR_EMPTY_INITIALISATION_LIST SR_ZERO_IS_SUCCESS SR_EXCLUDE_0_AND SR_ENABLE SR_DEBUG

 __stateReport()  Qnode::mmap_handler::openMap()

 @return The wrapped call has to return @e int . STATEREPORT passes this value unmodified.
         Strictly STATEREPORT does no return, STATEREPORT encapsulates the __stateReport()
         function which does the return.

 (fictive) calling @b examples
 \code
 // plain common call, report any non 0 return
 if ( STATEREPORT(foo(x)) ) {
   // action if a state is reportted
 }
 \endcode
 \code
 // get some handle, report < 0 (Errors) only
 if ( handle= STATEREPORT(open(x),'E'))< 0 ) {
   // action if a state is reportted
   ...
   return -1; // return a (error) state to the caller, the caller may use STATEREPORT to detect it
 }
 // else use handle
 \endcode
 \code
 // Do a call that may not able to handle all cases and may return any > 0 state, handle those here
 // but if 0 or other positive state was returned report nothing, report < 0 error states
 state= STATEREPORT(low(a,b,c,d),SR_EXCLUDE_0_AND(1,2,3),'E' );
 switch ( state ) {
   case 1:
        ....action1
     break;
   case 2:
        ....action2
     break;
   case 3:
        ....action3
     break;
   default:
     if ( state< 0 ) return -1; // error state to caller

     // by quiet about states > 3
 }
 return 0; // success to caller
 \endcode
 \code
 // Go through STATEREPORT but be quiet about unexpected states
 // This may be usefull during development stages in which report is made silent temporarely.
 (void) STATEREPORT( dontcare(), 'q');
 \endcode
 \code
  // other common complex example that will report all (_what is ommitted, the default = 3) unexpected states
  switch ( auto open_state= STATEREPORT( (px-> openMap("mymap"),SR_EXCLUDE_0_AND(1,2))) ) {
    case 0:
        // default execution path, nothing (open_state==0) to report or (special) handling
        // Do the next as "normal" assumed thing
        break;
    case 1:
        // ok, but handle state 1 here, it was excluded from STATEREPORT
        break;
    case 2:
        // ok, but handle state 2 here, it was excluded from STATEREPORT
        break;
    case -6:
        // -6 was also reported through STATEREPORT but it does not hinder you to handle it anyway

    default:  // not 0, 1, 2, -6
        // give up, state report has already given the standard error/notification message
        return -2;  // pass a error state of this function to the caller
  }
 \endcode
 \code
  bool must_be_space= STATEREPORT( isspace(my_char), {int(true)});

  \endcode

  Do also look at the references to understand how it is used.
 STATEREPORT is for methods and functions that return 0 on success, a <0 value to report a error
 and >0 to notify another condition but that is not a (fatal) error, just a case that COULD be handled
 the caller. STATEREPORT is initiated to standarize the way these states are reported to the user

 Note that STATEREPORT is intended for development and exceptional situations that are not as severe
 as when excption handling is required. You may still decide to throw a exception upon the returned (error) state.

 The logic of return codes does NOT sequentially (in source) enummerate error values (r<0) or notifications (r>0)
 the policy shall be incremental enummeration and leave existing return values as they are. The rationalle
 is that the callers may check the return value and determine actions on that. The meaning of the numbering (besides
 <0 | 0 | >0 ) is local and helps to pin point the exact location of the condition that triggered it.

 STATEREPORT is intended to have the following impact on the software implementations:
 - Consequent return values, <0 = error state, 0 = success state, >=notification that is (optionally) to be handled
 - Consequent data output via the parameter list of a function instead of using return
   eg. int foo(out_type* out_,in_type const& _in)
 - Decomposition of complex functions (or methods) into smaller modular ones that communicate control flow by states.

 The main goal of STATEREPORT macro (in all its overloaded forms) is to ALWAYS report unanticipated return states.
 This signifficantly reduces writing state handling code with sacrceficing code quality (by silently ignore possible unanticipated completions).
 So the name 'STATEREPORT' refers to ALWAYS reporting the state (when debugging: also pointing the source location)
 that was not anticipated or handled.
 So if a call is made and there is no unhandled error or unhandled notification reported by STATEREPORT,
 the developer (and possibly also the user) is ensured that nothing unexpected happened.
 Reporting can be full, partly or made silent as well any combination of states can be excluded.
 STATEREPORT is not intended, but can be used in cases where 0 is returned on error.

 STATEREPORT is used in low level code where usually slipping away with (presumed) harmless states is usually
 acceptable, the programmer is sure that certain error conditions never happen, or - more likely - he is to lazy to
 check all completion states.
 Exceptions and other exceptional state or error handling are often too dramatic to handle them and usual conditional
 program logic too labour intensive. Neither will STATEREPORT replace error handling by exceptions!
 STATEREPORT is somewhere  between and internal! Covering all completions (all states) not yet knowing wether it is
 -# handled
 -# unhandled but covered or
 -# a error. However, in debug mode it may help error handling mechanisms (e.g. exceptions) by reporting file path,
 line number and function. Analogy: If exception is a metal bullet, STATEREPORT is more a like a rubber bullet,
 it hurts but does not kill.

 Systematic multilevel use of STATEREPORT has a similar effect as a stack backtrace without need of exceptions or other
 more complex stack backtrace mechanisms.

 Programs using STATEREPORT may sometimes report errors that do not immediately effect the program behaviour.
 But they do not cheat or deny that something odd is going on. Because STATEREPORT returns error or warning numbers
 otherwhise unrevealed, the developper can do a better job in understanding and resolving the program behaviour.
 This helps to early discover and bugs before code is released, hence it improves the development productivity and
 software quallity.

 STATEREPORT is particularly effective in early stages of development. By the time the application reaches
 end users STATEREPORT will be triggered in (very) rare cases, ideally never (meaning that all calls run to
 success as anticipated).
 STATEREPORT can also help test design if the tests deliberately attempt to cause reporting by STATEREPORT to check
 that the program responds correctly to exceptional cases.

 STATEREPORT can be used if the possitive return value is not a state but some valid value ( e.g open returns a
 filehandle) by STATEREPORT(call,.../ * _what * /='E'), but use it - that way - with care! STATEREPORT assumes
 that all values are specific state values. The recommendation is to apply it only to routines which are part of the
 source code and all possible returned values can be seen/reviewed.



 @todo suggest to let STATEREPORT (also) log to a logfile.


 @remark Changes.
         The meaning of STATEREPORT has changed somewhat in a new code branch to not treat 0 specially. Instead 0 is a
         default exclusion. This allows to detect a unexpected return value 0 also in order to make STATEPORT more
         generic.

 @remark Within the Qnode package, methods and functions that return a int and have documented to return 'a state,...'
         (for example,  the return value description of openMap())
         are intended to be wrapped by STATEREPORT to detect unexpected (not exluded) states. A state is not defined as
         a type by design because the intend is to have and keep STATEREPORT as generic as possible. A state should be
         a (small) negative or positive natural number or zero. Any used return type should be convertable to int and
         still result in a valid handable state.

*/


#define SR_ENABLE 1    // 0 will eliminate all STATEREPORT calls
#define SR_DEBUG 1     // should be 0 for releases


//:SR_EXCLUDE_ALL
/**
 @brief wrap (int) value list, states which should not be reported
 @sa STATEREPORT

 Help @e ismpl to instrument correctly by avoiding '{' and '}' in the actual exclusion list during the
 use of STATEREPORT
*/
#define SR_EXCLUDE_ALL(...) std::vector<int>({ __VA_ARGS__ })


//:SR_EMPTY_INITIALISATION_LIST
/**
 @brief wrap empty value list
 @sa STATEREPORT
*/
#define SR_EMPTY_INITIALISATION_LIST SR_EXCLUDE_ALL()


//:SR_ZERO_IS_SUCCESS
/**
 @brief exclusion vector with only 0 in it, used as default on overloaded __stateReport() without exclusions
 @param[in] ... list of natural numbers representing a handled or ignored state that is excluded from a state report
 @sa STATEREPORT SR_EXCLUDE_ALL

 So in fact there is a single exclusion, that is 0 to indicate a success. So STATEREPORT without specified exclusions
 will always ignore a 0 return value.

 @remark Historically STATEREPORT interpreted 0 as a success value hardcoded. But now it is a default or macro assisted
         common success value.
*/
#define SR_ZERO_IS_SUCCESS SR_EXCLUDE_ALL(0)

//:SR_EXCLUDE_0_AND
/**
 @brief wrap (int) value list, states which should not be reported and include 0 (the default success value)
 @param[in] ... list of natural numbers representing a handled or ignored state that is excluded from a state report
 @sa STATEREPORT SR_EXCLUDE_ALL
 */
#define SR_EXCLUDE_0_AND(...) SR_EXCLUDE_ALL(0,__VA_ARGS__)


//:__stateReport
#if SR_ENABLE
# if SR_DEBUG
/**
 @brief enabled:debug: implements the operations for macro STATEREPORT

 @sa STATEREPORT
 @param _return_state_of_call_ return value (state) of _caller
 @param _callee the method or routine that is called
 @param _file is generated by STATEREPORT expansion, showing the source file path
 @param _line is generated by STATEREPORT expansion, showing the line in the source file
 @param _caller is generated by STATEREPORT expansion to for presenting
                the origin in case a unexpected state needs to be reported.
 @param _exclusionsVector list of @e int's for which no reporting is required, often states handled in a switch
        after calling the callee.
 @param _what  0, 1, 2  or  3  (default=3 meaning report everything)
 @warning Do not call __stateReport() directly ( that is why the name starts with __ )  use macro STATEREPORT
*/
int
__stateReport( int _return_state_of_call_, char const* _callee, char const* _file, unsigned _line, char const* _caller
             , std::vector<int> const& _exclusionsVector=SR_ZERO_IS_SUCCESS, int _what=3
             );

/**
 @brief enabled:debug: implements the operations for macro STATEREPORT
 @sa STATEREPORT
 @param _return_state_of_call_ return value (state) of _caller
 @param _callee the method or routine that is called
 @param _file is generated by STATEREPORT expansion, showing the source file path
 @param _line is generated by STATEREPORT expansion, showing the line in the source file
 @param _caller is generated by STATEREPORT expansion to for presenting
                the origin in case a unexpected state needs to be reported.
 @param _what  0, 1, 2  or  3  (default=3 meaning report everything)
 @warning Do not call __stateReport() directly ( that is why the name starts with __ ) use macro STATEREPORT
*/
int inline __stateReport( int _return_state_of_call_, char const* _callee, char const* _file, unsigned _line
                        , char const* _caller, int _what)
{
    return __stateReport( _return_state_of_call_, _callee, _file, _line, _caller, SR_ZERO_IS_SUCCESS, _what);
}
#  define STATEREPORT(_CALL,...) \
          util::__stateReport(_CALL, #_CALL, __FILE__, __LINE__, __PRETTY_FUNCTION__,##__VA_ARGS__)
# else  // no SR_DEBUG

int
/**
 @brief enabled: NO debug: implements the operations for macro STATEREPORT
 @sa STATEREPORT
 @param _return_state_of_call_ return value (state) of _caller
 @param _callee the method or routine that is called
 @param _caller is generated by STATEREPORT expansion to for presenting
                the origin in case a unexpected state needs to be reported.
 @param _exclusionsVector list of @e int's for which no reporting is required, often states handled in a switch
        after calling the callee.
 @param _what  0, 1, 2  or  3  (default=3 meaning report everything)  or 'q', 'n', 'e', 'a' or 'Q', 'N', 'E' or 'A'

 @warning Do not call __stateReport() directly ( that is why the name starts with __ ) use macro STATEREPORT
*/
__stateReport( int _return_state_of_call_, char const* _callee, char const* _caller
             , std::vector<int> const& _exclusionsVector=SR_ZERO_IS_SUCCESS, int _what=3
             );
/**
 @brief enabled: NO debug: implements the operations for macro STATEREPORT
 @param _return_state_of_call_ return value of _caller
 @param _callee the method or routine that is called
 @param _what  0, 1, 2  or  3  (default=3 meaning report everything) or 'q', 'n', 'e', 'a' or 'Q', 'N', 'E' or 'A'
 @sa STATEREPORT
 @warning Do not call __stateReport() directly ( that is why the name starts with __ )  use macro STATEREPORT
*/
int inline __stateReport(  int _return_state_of_call_, char const* _callee, char const* _caller,int _what)
{
    return __stateReport( _return_state_of_call_, _callee, _caller, SR_ZERO_IS_SUCCESS, _what);
}
#  define STATEREPORT(_CALL,...) util::__stateReport(_CALL, #_CALL, __PRETTY_FUNCTION__,##__VA_ARGS__)
# endif // SR_DEBUG
#else // no SR_ENABLE
# define STATEREPORT(_CALL,...) _CALL
#endif // SR_ENABLE

//}_____________________________________________________________________________________________________________________


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
