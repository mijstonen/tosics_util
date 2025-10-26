#!/usr/bin/env cpx

// TODO: integrate into cpx/util tests

#!
    // expecting filedescriptor >= 0  so any negative value denotes a error
    // see: man 2 open, man 2 stat, man 1 chmod
    int handle= open("tu_setfilesize.dat", ( O_CREAT | O_TRUNC | O_RDWR ), mode_t(/*octal*/0666)  );
    if ( STATEREPORT( handle,'E')<0 ) {
        CERROR("open(\"tu_setfilesize.dat\") failed");
        (void)Error_close( handle, strerror( errno));
    }
    else {
        auto no_0_from_SetFileSize= STATEREPORT( SetFileSize( handle, 2345) );
        INFO(VARVALS(handle,no_0_from_SetFileSize));
        if ( no_0_from_SetFileSize ) {
            CERROR("Unexpected value:",VARVALS(no_0_from_SetFileSize));
        }
        INFO("\nHee, go and look for tu_setfilesize.dat is 2345 bytes in size. try ls -l --block-size=1 tu_setfilesize.dat");
    }

