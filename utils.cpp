#include "utils.hpp"

namespace util {

/*
________________________________________________________________________________________________________________________
delegated from STATEREPORT macro, see its description in utils.h
*/
#if SR_ENABLE
# if SR_DEBUG
int __stateReport(  int _return_state_of_call_, char const* _callee, char const* _file, unsigned _line, char const* _caller, std::vector<int> const& _exclusionsVector, int _what)
# else // release hides location information
int __stateReport(  int _return_state_of_call_, char const* _callee, char const* _caller, std::vector<int> const& _exclusionsVector, int _what)
# endif // SR_DEBUG
{
#if 0
    if  ( !_return_state_of_call_ )                                          goto LB_REPORT_SKIPPED;  // Ok
    for ( auto ve : _exclusionsVector )   if ( ve== _return_state_of_call_ ) goto LB_REPORT_SKIPPED;  // Handled in caller
#else
    // No special treatment when _return_state_of_call_ 0, instead by default the _exclusionsVector is not empty,
    // but contains int(0). Therefor its use is going in cases where the _exclusionsVector is exclusive.
    // The objective is to be able to also consider 0 as a unexpected state (fa. only positieve values allowed)
    for ( auto ve : _exclusionsVector ) {
        if ( ve== _return_state_of_call_ ) {
            goto LB_REPORT_SKIPPED;  // Handled in caller
        }
    }
#endif

    // translate option character
    switch ( (static_cast<char>(tolower( _what))) )
    {
        // cases in order of (estimated) probabillity
        case 'a': _what=3; break;  // All
        case 'e': _what=2; break;  // Error
        case 'n': _what=1; break;  // Notify
        case 'q': _what=0; break;  // be Quiet
        default:
            // heal some other error prone value (better then reporting error in error handling)
            if ( ( _what< 0 )|| ( _what> 3 ) ) {
                _what= 3; // All
            }
            // else values 0, 1, 2, 3 remain
    }
# if SR_DEBUG
    #define  STD_STATE_MSG CERROR(VARVAL(kind),VARVAL(_return_state_of_call_),VARVAL(_callee),VARVAL(_file),VARVAL(_line),VARVAL(_caller))
# else
    #define  STD_STATE_MSG CERROR(VARVAL(kind),VARVAL(_return_state_of_call_),VARVAL(_callee),VARVAL(_caller))
# endif
    if ( ( _what& 0x1 )&& ( _return_state_of_call_>= 0  ) ) { // some other notification
        char const* kind="Unhandled NOTIFICATION";
        STD_STATE_MSG;
    }
    if ( ( _what& 0x2 )&& ( _return_state_of_call_< 0 ) ) { // error
        char const* kind="Unhandled ERROR";
        STD_STATE_MSG;
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
    if ( STATEREPORT(  close( _fd)  ) ){
        perror("close() failed");
        return -1;
    }
    return 0;
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
    if ( ofs== static_to_type_cast( ofs,-1) ) {
        Error_close( _fd, "Error calling lseek() to 'stretch' the file");
        return -1;
    }
    ASSERT( static_to_type_cast(new_pos, ofs)== new_pos  );

    if ( _grow ) {
        // write last byte causes file size to be set to newsize+1 ( == _size )
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
    }
    return 0;
}
/*
________________________________________________________________________________________________________________________
*/



}// namespace util



