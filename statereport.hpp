#pragma once
//statereport.hpp
#ifndef STATEREPORT_H_
#  define STATEREPORT_H_ 1

#include "util.hpp"
#include "info.hpp"
#include "preserve.hpp"
#include "error.hpp"

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

namespace tosics::util {

    //:STATEREPORT
/**
 \def STATEREPORT

 @brief debugging aid to have a standard way to report any uncontrolled and unexpected return state of a low(er)
        level called function. Particular to check anyway in cases where normally you would ignore error or warning
        states by function return values.

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
  - 2 'e' 'E'  only report errors (negative return statements)
  - 3 'a' 'A'  report all, note that this is used rarely explicitly because if _what is ommited then 3 is asumed

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
   // action if a state is reported
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

#  ifndef SR_ENABLE
#    define SR_ENABLE 1    // 0 will eliminate all STATEREPORT calls
#  endif
#  define SR_DEBUG DEBUG     // should be 0 for releases


//:SR_EXCLUDE_ALL
/**
 @brief wrap (int) value list, states which should not be reported
 @sa STATEREPORT

 Help @e ismpl to instrument correctly by avoiding '{' and '}' in the actual exclusion list during the
 use of STATEREPORT
*/
#  define SR_EXCLUDE_ALL(...) std::vector<tosics::util::state_t>({ __VA_ARGS__ })


//:SR_EMPTY_INITIALISATION_LIST
/**
 @brief wrap empty value list
 @sa STATEREPORT
*/
#  define SR_EMPTY_INITIALISATION_LIST SR_EXCLUDE_ALL()


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
#  define SR_ZERO_IS_SUCCESS SR_EXCLUDE_ALL(0)

//:SR_EXCLUDE_0_AND
/**
 @brief wrap (int) value list, states which should not be reported and include 0 (the default success value)
 @param[in] ... list of natural numbers representing a handled or ignored state that is excluded from a state report
 @sa STATEREPORT SR_EXCLUDE_ALL
 */
#  define SR_EXCLUDE_0_AND(...) SR_EXCLUDE_ALL(0,##__VA_ARGS__)


//:__stateReport
#  if SR_ENABLE
#    if SR_DEBUG
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
    state_t
__stateReport( state_t _return_state_of_call_, char const* _callee, char const* _file, unsigned _line, char const* _caller
             , std::vector<state_t> const& _exclusionsVector=SR_ZERO_IS_SUCCESS, int _what=3
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
    inline state_t
__stateReport( state_t _return_state_of_call_, char const* _callee, char const* _file, unsigned _line
                        , char const* _caller, int _what)
{
    return __stateReport( _return_state_of_call_, _callee, _file, _line, _caller, SR_ZERO_IS_SUCCESS, _what);
}





#      define STATEREPORT(_CALL,...) \
          tosics::util::__stateReport(_CALL, #_CALL, __FILE__, __LINE__, __PRETTY_FUNCTION__,##__VA_ARGS__)




#    else  // no SR_DEBUG

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
    state_t
__stateReport( state_t _return_state_of_call_, char const* _callee, char const* _caller
             , std::vector<state_t> const& _exclusionsVector=SR_ZERO_IS_SUCCESS, int _what=3
             );





/**
 @brief enabled: NO debug: implements the operations for macro STATEREPORT
 @param _return_state_of_call_ return value of _caller
 @param _callee the method or routine that is called
 @param _what  0, 1, 2  or  3  (default=3 meaning report everything) or 'q', 'n', 'e', 'a' or 'Q', 'N', 'E' or 'A'
 @sa STATEREPORT
 @warning Do not call __stateReport() directly ( that is why the name starts with __ )  use macro STATEREPORT
*/
    state_t inline
__stateReport(  state_t _return_state_of_call_, char const* _callee, char const* _caller,int _what)
{
    return __stateReport( _return_state_of_call_, _callee, _caller, SR_ZERO_IS_SUCCESS, _what);
}





#      define STATEREPORT(_CALL,...) tosics::util::__stateReport(_CALL, #_CALL, __PRETTY_FUNCTION__,##__VA_ARGS__)
#    endif // SR_DEBUG






/** @brief Set, get, or exchange the stream pointer to the stream that is used during STATEREPORT
 *  @param _stateReport_StreamPtr New stream pointer (aka &std::cout or &my_file_stream)
 *         If _stateReport_StreamPtr==nullptr  it will not alter the existing stream pointer, see return value.
 * @return Returns the stream pointer before it was altered. If _stateReport_StreamPtr==nullptr, this function
 *         can be used to get the stream pointer without setting another.
 *
 * STATEREPORT has its sole purpose during development. When the application is in production, it should still check
 * but it is by then expected that the checks always pass. However if that is not the case a enduser could be
 * disturbed by messages that where not intended for him. To avoid this problem, this function can redirect the
 * STATEREPORT messaages to another stream, aka a logfile or a dead null stream. By default STATEREPORT reports to
 * the standard error stream.
 *
 * @remark This function does not create a stream. The creation of a alternate stream is left to the user application
 *         because other operations to that same stream could be applied.
 * @remark Usually the state report stream pointer should only be set once in program and restoring should not be
 *         needed. However is modify locally, the LOCAL_MODIFIED macro provides eleganter method to temporarely alter
 *         the stream pointer (as shown in the demo).
*/
extern thread_local std::ostream* stateReport_StreamPtr;

std::ostream* stateReport_exchange_StreamPtr(std::ostream* _stateReport_StreamPtr=nullptr);
#  else // no SR_ENABLE
#    define STATEREPORT(_CALL,...) _CALL
#  endif // SR_ENABLE

} //namespace util

#endif // STATEREPORT_H_
