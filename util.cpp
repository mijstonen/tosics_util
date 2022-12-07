#include <tosics_util.hpp>

namespace tosics::util {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
const void* SCX_MAP_FAILED=   ((const void*)-1);// == MAP_FAILED  but type is const void*
#pragma GCC diagnostic pop


/*
_______________________________________________________________________________________________________________________
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
        case 'a': _what=3; break;  // All -> Error, Notify and zero
        case 'e': _what=2; break;  // Error and zero
        case 'n': _what=1; break;  // Notify and zero
        case 'q': _what=0; break;  // be Quiet
        default:
            // heal some other error prone value (better then reporting error in error handling)
            if ( ( _what< 0 )|| ( _what> 3 ) ) {
                LOCAL_MODIFIED(INFO_STREAM_PTR);
                INFO_TO(*stateReport_StreamPtr);
                INFO(FUNC_MSG(HYELLOW "Healing "<<VARVALS(_what)<<" to 3 " NOCOLOR));
                INFO_STREAM_PTR->flush();
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
        LOCAL_MODIFIED(INFO_STREAM_PTR);
        INFO_TO(*stateReport_StreamPtr);
        char const* MessageColor= ( mode==2 )?HYELLOW :HRED;
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
        CERROR("PANIC: Too many backtrace buffers in tosics::util::DumpBacktraceInFileStream(), aborting!");
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

    state_t // lambda
LeftShiftOut_First_ProgramArgument(std::vector<std::string>::size_type _number_of_additiional_parameters)
{
    decltype(_number_of_additiional_parameters) minimum_ProgramArguments_size(2); /* 1+1  PA[0] remains, always shift PA[1]*/
    minimum_ProgramArguments_size=+ _number_of_additiional_parameters;
    if ( EXPECT_false_FROM(ProgramArguments.size()< minimum_ProgramArguments_size)) {
        return State(-1);
    }
    auto starting_position= ProgramArguments.begin()+ 1; // skip program name at ProgramArguments[0]
    auto end_position_not_included= starting_position + _number_of_additiional_parameters+ 1;

#define VECTOR_REQUIREMENT_OK 1   // REQUIREMENT: ProgramArguments has to be a vector where begin()<end() and ++ increases value
#if VECTOR_REQUIREMENT_OK
    if ( EXPECT_false_FROM(end_position_not_included> ProgramArguments.end()) ) {
        // number of items to erease is more then items in the ProgramArguments
        return State(-2);
    }
//#else first condition return State(-1) should guard the operation sufficiently
#endif
#undef VECTOR_REQUIREMENT_OK
    ProgramArguments.erase( starting_position, end_position_not_included);
    return State(0);
};


    void
Initialize(int _argC, char const* _argV[])
{
    signal(SIGSEGV, On_signal);
    for( int argI=0; argI<_argC; ++argI ){
        ProgramArguments.emplace_back(_argV[argI]);
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



namespace fs=std::filesystem;

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
            if ( canonical_path_ ) {
                ( *canonical_path_ )= fs::canonical( gues_path);
            }
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
         ( *time_ ) = std::chrono::system_clock::to_time_t( std::chrono::file_clock::to_sys(ftime));
    }
    catch ( ... ) {
        return State(-1); // probably: filesystem_error, _path not found, but catch any exception here
    }
    return State(0);
}



namespace {
}

uint32_t LittleEndianCrc32Table[256] = {
        0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988, 0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
        0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7, 0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
        0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172, 0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
        0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924, 0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
        0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433, 0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
        0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e, 0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
        0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0, 0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
        0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f, 0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
        0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a, 0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
        0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc, 0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
        0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b, 0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
        0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236, 0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
        0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38, 0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
        0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777, 0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
        0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2, 0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
        0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94, 0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

    union
Endian
{
  private:
    const uint64_t i=0x7061524334251607;
    const uint8_t  cc[8];

  public:

      Endian(){}

    bool is_Little() const
    {
        decltype(cc) dd={0x07,0x16,0x25,0x34,0x43,0x52,0x61,0x70};
        return ! memcmp(cc,dd,sizeof cc);
    }
}
;

const bool Is_Little_Endian(Endian().is_Little());

    ConstReturn_OverloadTag
extern With_ConstReturn
;


}// namespace tosics::util



