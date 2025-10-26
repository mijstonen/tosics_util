#!/usr/local/bin/cpx

#!
    std::string path = ".";
    for (auto & p : fs::directory_iterator(path) ) {
        std::cout << p << std::endl;

        time_t epoch;
        if ( STATEREPORT(PathWriteTime( &epoch, p)) ){
            CERROR( "PathWriteTime() faied for ",VARVAL(p));
        }

        INFO(VARVALS(p,epoch));
    }

