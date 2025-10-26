#!/usr/bin/env cpx


// put your globals, templates,class definitions and includes here

namespace fs=std::experimental::filesystem;

#!
    fs::path canonical_path;
    // determine canonical_path
    if ( STATEREPORT(FileInPATH( &canonical_path, "filesystem_experiments.cpp",".")) ) {
        ThrowBreak("FileInPath() failed");
    }

    SHA1 s;
    s.processBytes( canonical_path);
    INFO(VARVAL(s.make_digest_string()));

