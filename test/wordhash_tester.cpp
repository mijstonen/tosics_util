#!/usr/local/bin/cpx


bool is_too_strange(string const& _word)
{
    if ( ! _word.length() ) return true;
    for (auto c: _word) {
        int as_code = static_cast<int>(static_cast<unsigned char>(c));
        if ( as_code < 32 ) return true;
        if ( as_code > 127 ) return true;
    }
    // passed
    return false;
}

map<uint64_t,string> Words;

void dumpWords()
{
    for ( auto const& [hash,word]: Words ) {
        INFO(VARVALS(hash,word));
    }
}

#!
    INFO(VARVAL(Cstr2uint64("")));

    size_t collisions(0);
    string word;
    while (getline(cin,word)) {
        if ( is_too_strange( word) )
     continue/*while(getline...)*/;// too strange word
        //else
        auto hash(Cstr2uint64(word));
        cout<<hash<<endl;

        if ( Words.find(hash)==Words.end() ) {
            Words[hash]= word;
     continue;/*while(getline...)*/;// new word was added
        }//else
        if ( Words[hash] == word )
     continue/*while(getline...)*/;// expected result
        //else
        ++collisions;
        CERROR("Collision:  ",VARVALS(collisions,word,hash,Words[hash]));
    }//while(getline...)
#if 1
    dumpWords();
#endif
INFO(VARVAL(collisions));

