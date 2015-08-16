#include "utils.hpp"
#include "info.hpp"
#include "preserve.hpp"
#include "error.hpp"
#include "statereport.hpp"


namespace util {

/*
________________________________________________________________________________________________________________________
delegated from STATEREPORT macro, see its description in utils.h
*/

#if SR_ENABLE

thread_local std::ostream* stateReport_StreamPtr= &std::cerr;

#if 1
std::ostream* stateReport_exchange_StreamPtr(std::ostream* _stateReport_StreamPtr)
{
    std::ostream* return_value= stateReport_StreamPtr;
    if ( _stateReport_StreamPtr ) {
        stateReport_StreamPtr= _stateReport_StreamPtr;
    }
    return return_value;
}
#endif

# if SR_DEBUG
long __stateReport(  long _return_state_of_call_, char const* _callee, char const* _file, unsigned _line, char const* _caller, std::vector<long> const& _exclusionsVector, int _what)
# else // release hides location information
long __stateReport(  long _return_state_of_call_, char const* _callee, char const* _caller, std::vector<long> const& _exclusionsVector, int _what)
# endif // SR_DEBUG
{
    short mode=0;
    char const* kind[]{ ""
                      , "ERROR"
                      , "NOTIFICATION"
                      , "ZERO/FALSE"
                      };
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
        LOCAL_MODIFIED(INFO_STREAM_PTR);
        INFO_TO(*stateReport_StreamPtr);
        INFO();
#define returning _return_state_of_call_
# if SR_DEBUG
        INFO("STATEREPORT>>>Unhandled ",kind[mode],VARVAL(returning),VARVAL(_callee),VARVAL(_file),VARVAL(_line),VARVAL(_caller))
# else
        INFO("STATEREPORT>>>Unhandled ",kind[mode],VARVAL(returning),VARVAL(_callee),VARVAL(_caller))
# endif
        ;
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
size_t align_size_to_multi_min( size_t _size_, size_t const& _min)
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
helper to de STATEREPORT() perror() on close()
*/
int Error_close( int _fd)
{
    if ( !STATEREPORT(  close( _fd)  ) ){
        return 0;
    }
    perror("close() failed");
    return -1;
}
/*
________________________________________________________________________________________________________________________
set file to desired size, if it needs to grow then the size is set by writing the last byte otherwhise the content
beyond the set size is lost.
*/
int setFileSize( int _fd, size_t _size, bool _grow)
{
    // set size
    auto new_pos= _size;
    if ( _grow ) {
      --new_pos;
    }

    auto ofs= lseek( _fd, static_cast<off_t>( new_pos), SEEK_SET);

    #if 0
    if ( ofs== static_to_type_cast( ofs,-1) ) {
        Error_close( _fd, "Error calling lseek() to 'stretch' the file");
        return -1;
    }
    #else   // rewrote code but it is not tested so on doubts, fallback to the code above. Note that the error output is different.

    if ( STATEREPORT( ofs, SR_EXCLUDE_ALL( static_cast<long>(new_pos)) )== -1 ) {
        STATEREPORT( Error_close( _fd, "Error calling lseek() to 'stretch' the file") );
        return -1;
    }
    #endif
    ASSERT( static_to_type_cast(new_pos, ofs)== new_pos  );

    if ( _grow ) {
        // write last byte causes file size to be set to newsize+1 ( == _size )
        #if 0
        auto wr= write( _fd, "", 1);
        if ( wr< static_to_type_cast(wr,0) ) {
            if (STATEREPORT( wr , 'E' ))  {
                Error_close( _fd, "Error writing last byte of the file");
                return -2;
            }
        }
        else if ( wr!= static_to_type_cast(wr,1) ) {  // write() did not return a error code but a incorrect number of bytes written
            Error_close( _fd);
            std::cerr<< "write() did write "<<wr << "bytes in stead of one last byte"<<std::endl;
            return -3;
        }
        #else   // rewrote code but it is not tested so on doubts, fallback to the code above. Note that the error output is different.
        auto write_last_byte = write( _fd,"", 1);
        if ( STATEREPORT(write_last_byte,SR_EXCLUDE_ALL(1))!=1 ) {
            CERROR(VARVAL(write_last_byte)," expected to be '1'");
            STATEREPORT( Error_close( _fd));
        }
        #endif
    }
    return 0;
}
/*
________________________________________________________________________________________________________________________
*/

static thread_local InfoSettings StandardInfoSettings;
InfoSettings *AppliedInfoSettingsPtr= &StandardInfoSettings;

    int
InfoSettings::validateFailed() const
{
    if ( ostreamPtr==nullptr ) {
        return -1;
    }
    if ( objectSeparation==nullptr ) {
        return -2;
    }
    if ( customQuote==nullptr ) {
        return -3;
    }

    if ( strlen(objectSeparation)<1 ) {
        return -21;
    }
    if ( strlen(customQuote)<1 ) {
        return -22;
    }

    return 0;
}

}// namespace util



