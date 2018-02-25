#include "util.hpp"
#include "info.hpp"
#include "preserve.hpp"
#include "error.hpp"
#include "statereport.hpp"
#include "member.hpp"


namespace tosics::util {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
const void* SCX_MAP_FAILED=   ((const void*)-1);// == MAP_FAILED  but type is const void*
#pragma GCC diagnostic pop


/*
________________________________________________________________________________________________________________________
delegated from STATEREPORT macro, see its description in utils.h
*/

#if SR_ENABLE

thread_local std::ostream* stateReport_StreamPtr= &std::cerr;

#if 1
    std::ostream*
stateReport_exchange_StreamPtr(std::ostream* _stateReport_StreamPtr)
{
    std::ostream* return_value= stateReport_StreamPtr;
    if ( _stateReport_StreamPtr ) {
        stateReport_StreamPtr= _stateReport_StreamPtr;
    }
    return return_value;
}
#endif

# if SR_DEBUG
    state_t
__stateReport(  state_t _return_state_of_call_, char const* _callee, char const* _file, unsigned _line, \
                char const* _caller, std::vector<state_t> const& _exclusionsVector, int _what)
# else // release hides location information
    state_t
__stateReport(  state_t _return_state_of_call_, char const* _callee, \
                char const* _caller, std::vector<state_t> const& _exclusionsVector, int _what)
# endif // SR_DEBUG
{
    static char const*const kind[]{ ""
                      , "ERROR"
                      , "NOTIFICATION"
                      , "OK: State(0), exclude it from STATEREPORT!"  // set _exclusionsVector to SR_ZERO_IS_SUCCESS
                      };
    short mode=0;
    // No special treatment when _return_state_of_call_ 0, instead by default the _exclusionsVector is not empty,
    // but contains int(0). Therefor its use is going in cases where the _exclusionsVector is exclusive.
    // The objective is to be able to also consider 0 as a unexpected state (fa. only positieve values allowed)
    for ( auto ve : _exclusionsVector ) {
        if ( ve== _return_state_of_call_ ) {
            goto LB_REPORT_SKIPPED;  // Handled in caller
        }
    }

    // translate option character
    switch ( (static_cast<char>(tolower( _what))) )
    {
        // cases in order of (estimated) probabillity
        case 'a': _what=3; break;  // All
        case 'e': _what=2; break;  // Error and zero
        case 'n': _what=1; break;  // Notify and zero
        case 'q': _what=0; break;  // be Quiet
        default:
            // heal some other error prone value (better then reporting error in error handling)
            if ( ( _what< 0 )|| ( _what> 3 ) ) {
                _what= 3; // All
            }
            // else values 0, 1, 2, 3 remain
    }

    if ( _return_state_of_call_< 0 ) {
        if ( _what& 0x2 ) {
            mode=1;
        }
    }
    else if (_return_state_of_call_> 0 ) {
        if ( _what& 0x1 ) {
            mode=2;
        }
    }
    else {
        assert( _return_state_of_call_==0 );                // Precondition that shall be met
        if ( _what ) {
            mode=3;
        }
    }
    if ( mode ) {
        char const* MessageColor= ( mode==2 )?HYELLOW :HRED;
        LOCAL_MODIFIED(INFO_STREAM_PTR);
        INFO_TO(*stateReport_StreamPtr);
        INFO(MessageColor);     // combined 1. MessageColor and 2. a newline before writing
        std::string state_msg="State("+std::to_string(_return_state_of_call_)+")";
#define returning _return_state_of_call_
# if SR_DEBUG
        INFO("STATEREPORT>>>Unhandled ",kind[mode],VARVAL(returning),VARVAL(state_msg),VARVAL(_callee),VARVAL(_file), \
                VARVAL(_line),VARVAL(_caller));
# else
        INFO("STATEREPORT>>>Unhandled ",kind[mode],VARVAL(returning),VARVAL(state_msg),VARVAL(_callee),VARVAL(_caller));
# endif
        INFO(NOCOLOR);    // combined 1. Clear color changes and 2. a newline before writing
        INFO_STREAM_PTR->flush();
#undef returning
    }
    LB_REPORT_SKIPPED:
    return _return_state_of_call_;
}//__stateReport()
#endif // SR_ENABLE
/*
________________________________________________________________________________________________________________________
universal alignment function
*/
    size_t
AlignSizeToMultiMin ( size_t _size_, size_t const& _min)
{
  size_t bytes_exceeding_last_min= _size_ % _min;

  if ( bytes_exceeding_last_min ) {
    // add distance to next multi min to size
    _size_ += ( _min- bytes_exceeding_last_min );
  }
  // else..
  // .. _size_ is already aligned
  //

  ASSERT( _size_==0 || ( _size_ / _min ) >= 1 ); // _size_ MUST be a multiple of _min
  ASSERT( !( _size_ % _min ) );                   // there should be no rest value

  return _size_;
}

/*
________________________________________________________________________________________________________________________
helper to the STATEREPORT() perror() on close()
*/
    state_t
Error_close( int _fd)
{
    if ( !STATEREPORT(  close( _fd)  ) ){
        return State(0);
    }
    perror("close() failed");
    return State(-1);
}
/*
________________________________________________________________________________________________________________________
set file to desired size, if it needs to grow then the size is set by writing the last byte otherwhise the content
beyond the set size is lost.


REMARKS
  use         open(<const char* filename string>, ( O_CREAT | O_TRUNC | O_RDWR ), mode_t( \*octal*\ 0666)  );
  to set _fd

  This function is not crossplatform portable.
*/
    int
SetFileSize ( int _fd, size_t _size, bool _grow)
{
    // set size
    auto new_pos= _size;
    if ( _grow ) {
      --new_pos;
    }

    auto ofs= lseek( _fd, static_cast<off_t>( new_pos), SEEK_SET);

    if ( STATEREPORT( ofs, SR_EXCLUDE_ALL( static_cast<state_t>(new_pos)) )== -1 ) {
        STATEREPORT( Error_close( _fd, "Error calling lseek() to 'stretch' the file") );
        return -1;
    }
    ASSERT( static_to_type_cast(new_pos, ofs)== new_pos  );

    if ( _grow ) {
        auto write_last_byte = write( _fd,"", 1);
        if ( STATEREPORT(write_last_byte,SR_EXCLUDE_ALL(1))!=1 ) {
            CERROR(VARVAL(write_last_byte)," expected to be '1'");
            STATEREPORT( Error_close( _fd, "write() call failed"));
            return -2;
        }
    }
    return 0;
}
/*
________________________________________________________________________________________________________________________
*/

static thread_local InfoSettings StandardInfoSettings;
InfoSettings thread_local *AppliedInfoSettingsPtr= &StandardInfoSettings;

    state_t
InfoSettings::validateFailed() const
{

    //TODO : more checks required, also check recent added

    if ( ostreamPtr==nullptr ) {
        return State(-1);
    }
    if ( objectSeparation==nullptr ) {
        return State(-2);
    }
    if ( customQuote==nullptr ) {
        return State(-3);
    }

    if ( strlen(objectSeparation)<1 ) {
        return State(-21);
    }
    if ( strlen(customQuote)<1 ) {
        return State(-22);
    }

    if ( objOnLineCnt ) {
        return State(1);
    }
    if ( hex_once ) {
        return State(2);
    }

    return State(0);
}
    char const*
DateTime()
{
    auto now = time(nullptr);
    return asctime(localtime(&now));
}
    void
Info_main_args (int argc, char const* argv[])
{
    for (int i = 0; i < argc; ++i) {
        INFO(VARVAL(i), VARVAL(argv[i]));
    }
}

//:DumpBacktraceInFileStream:// show backtrace
    void
DumpBacktraceInFileStream(int backtrace_output_filehandle_)
{
    // TODO: Fix missing symbols. Do name unmangeling.
    // Consider the new (since 1.65) Boost.Stacktrace  library.

    static bool entered=false; // avoid recursing
    static void* btbufs[MAX_BT_BUFS];

    if ( entered ) {
        // Should not be called inside it self (aka by signal, throw* function)
        CERROR("PANIC: Re-entered tosics::util::DumpBacktraceInFileStream(), aborting!");
        abort();
        return;
    }

    //else
    entered=true;
    int btbufs_used = backtrace(btbufs, MAX_BT_BUFS);
    if ( btbufs_used >= MAX_BT_BUFS ) {
        CERROR("PANIC: To many backtrace buffers in tosics::util::DumpBacktraceInFileStream(), aborting!");
        abort();
        return;
    }

    //else
    backtrace_symbols_fd(btbufs, btbufs_used, backtrace_output_filehandle_);
}

//:zipLeftsAndRightsJoin2Str:// Assist VARVALS, but generalized to zip 2 string vectors together
    state_t
zipLeftsAndRightsJoin2Str(
  std::string *out_
, std::vector<std::string> const& _lefts
, char const* _left_right_separator
, std::vector<std::string> const& _rights
, char const* _zippedSeparator
)
{
    state_t return_value= State(0);
    //Preconditions
    if ( !_lefts.size() ) {
        // no labels, must have something to be zipped
        return State(-1);
    }
    if ( !_rights.size() ) {
        // no values, must have something to be zipped
        return State(-2);
    }
    if ( _rights.size()!=_lefts.size() ) {
        // number of values must be equal to number of labels to prefix each value with a label
        return State(-3);
    }
    if ( !out_ ) {
        // output is mandatory
        return State(-4);
    }
    if ( out_->length() ) {
        // out_ will be overwritten, it is no error but that might be not intended, however the proces continues
        return_value= State(1);
    }

    std::vector<std::string> to_be_joined;
    for( decltype( _lefts.size()) i=0; i<_lefts.size(); ++i ) {
        std::vector<std::string> label_and_value= { move( _lefts[i]), move( _rights[i])};
        std::string itemStr;
        if ( STATEREPORT(   Append_joined( &itemStr, label_and_value, _left_right_separator)   ) ) {
            return State(-5);
        }
        to_be_joined.push_back( move( itemStr));
    }

    if ( STATEREPORT(   Append_joined( out_, to_be_joined, _zippedSeparator)   ) ) {
        return State(-6);
    }

    return return_value;
}

namespace {
    static char const * const varvals_ErrorMsgPart="From VARVALS(...) macro, varvals()/";
}

    void
__varvals_merge_with_lables(std::string* out_, char const * _args_str, std::vector<std::string> const& _values)
{
    // extract lables
    std::vector<std::string> labels;
    {state_t splitting_returns_no_0=
        Append_splitted( &labels, _args_str);
    if ( splitting_returns_no_0 ){
        ThrowBreak( std::logic_error( std::string(varvals_ErrorMsgPart) +
                                      "Append_splitted() returned State(" +
                                      std::to_string(splitting_returns_no_0) +
                                      ") : Failed splitting argument name list into separate arument names")
                  , eBC_default );
    }}

    // zip all labels and values to string
    {state_t zipjoin_returns_no_0=
        zipLeftsAndRightsJoin2Str( out_, labels,"=", _values, ObjectSeparation());
    if ( zipjoin_returns_no_0 ) {
        ThrowBreak( std::logic_error( std::string(varvals_ErrorMsgPart) +
                                      "zipLeftsAndRightsJoin2Str() returned State(" +
                                      std::to_string(zipjoin_returns_no_0) +
                                      ") : Failed zipping argument names and argument values together")
                  , eBC_default );
    }}
}

// Modern C++ style arguments
// + Globally visible
// + No need to repeat typing int _argC, char const* _argV  (or vector<string> ProgramArguments
// + Can be queried and maniputlated in STL style (std::vector and std::string)
// + Modern C++
// - Extra resource usage
    std::vector<std::string>
ProgramArguments;

    void
Info_ProgramArguments()
{
    size_t argCnt{0};
    for(auto argVal: ProgramArguments) {
        INFO(VARVALS(argCnt,ProgramArguments[argCnt]));
        ++argCnt;
    }
}
    void
On_signal(int _signal)
{
    psignal(_signal,__func__);
    CERROR(VARVAL(_signal));
    tosics::util::DumpBacktraceInFileStream();
    abort();
}
    void
Initialize(int _argC, char const* _argV[])
{
    signal(SIGSEGV, On_signal);
    for( int argI=0; argI<_argC; ++argI ){
        ProgramArguments.emplace_back(_argV[argI]);
    }
}

namespace fs=std::experimental::filesystem;

//:FileInPATH:// Look for file (probably a shell level command) in $PATH or list of directories
    state_t
FileInPATH ( fs::path* canonical_path_,  fs::path _filename, char const* _dirs)
{
    if ( Is_null ( _dirs) ) {
        char const* envvar_path_content= getenv("PATH");
        if ( Is_null (envvar_path_content) ) {
            return State(-1); // Could not retrieve the PATH environment variable
        }
        _dirs= envvar_path_content;
    }
    std::vector<std::string> directories;
    if ( STATEREPORT(Append_splitted( &directories, std::string(_dirs), ":")) ) {
        return State(-2); // Error during splitting.
    }

    for ( auto dir : directories ) {
        fs::path gues_path= fs::path(dir) / _filename;

        if ( fs::exists(gues_path) ) {
            ( *canonical_path_ )= fs::canonical( gues_path);
            return State(0);
        }
    }
    return State(-3); // File not found in any of the directories of the PATH environment variable.
}

//:PathWriteTime:// Give write time in seconds (since 1970, might be platform depended), used to detect changes.
    state_t
PathWriteTime(std::time_t *time_,fs::path _path)
{
    try {
        auto ftime = fs::last_write_time( _path);
        ( *time_ )= decltype(ftime)::clock::to_time_t(ftime);
    }
    catch ( ... ) {
        return State(-1); // probably: filesystem_error, _path not found, but catch any exception here
    }
    return State(0);
}

}// namespace util



