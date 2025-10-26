#!/usr/local/bin/cpx
#+ queue.hpp

 namespace {

    struct
 RunConcurrent
 {
    template<invocable..._BlocksPF>
    RunConcurrent(_BlocksPF..._blocks)
    {
            jthread
        concurrent_running_threads[]=
            {
                jthread(_blocks)...
            }
        ;
    }
 }
 ;
    bool
 LineFilter(const char *cs)
 {
     unsigned index=0;
     while(index=static_cast<unsigned char>(*cs), index) {
         if ( *cs<33 )  return false;
         if ( *cs>122 ) return false;
         ++cs;
     }
     return true;
 }

 } // namespace

#!
__________ components and queues __________

    Queue<string>
 wordDispatch(4096)
 ;
    Queue< pair<uint64_t,string> >
 hashCollect(4096)
 ;

    auto
 reader=
    [ file = "/home/michel/Projects/testdata/all-ascii-unique-words.txt"
    , word_chout=channel::send<string>(wordDispatch)
    ] // mutable -> void
    {
            ifstream
        input_stream(file)
        ;
        while ( input_stream && word_chout ) {
                channel::send<string>::data_type
            line_buffer
            ;
            getline( input_stream, line_buffer);
            word_chout<< move( line_buffer);
        }
    }
 ;
    auto
 worker=
    [   word_chinp = channel::recv<string>(wordDispatch)
    ,   hw_chout   = channel::send<pair<uint64_t,string>>(hashCollect)
    ]() // mutable -> void
    {
        this_thread::yield();
        while (word_chinp && hw_chout) {
                channel::send<pair<uint64_t,string>>::data_type
            hash_and_word
            ;
            word_chinp>> hash_and_word.second;
            if ( LineFilter(hash_and_word.second.c_str()) ) {
                hash_and_word.first= Cstr2uint64( hash_and_word.second);
                hw_chout<< move( hash_and_word);
            }
            //this_thread::yield();  // lets get input delivered by other threat(s) first
        }
    }
 ;
    auto
 collector=
    [ hw_chinp = channel::recv<pair<uint64_t,string>>(hashCollect)
    ]() // mutable -> void
    {
            map<uint64_t,vector<string>>
        whmap
        ;
        this_thread::yield();
        while ( hw_chinp ) {
                channel::recv< pair<uint64_t,string>>::data_type
            hash_and_word
            ;
            hw_chinp>> hash_and_word;
            //whmap.insert(  hash_and_word);
            whmap[/*hash*/hash_and_word.first].push_back(/*word*/hash_and_word.second);
        }

        INFO(ENDL,"______________________________________________________________");

        for(auto [k,v]:whmap) {
            if ( v.size()!=1 ) {
                INFO(VARVALS(k,v));
            }
        }
    }
 ;

__________ master thread __________
 INFO("running");
 //reader();

      jthread
   r(reader)
 , w1(worker), w2(worker), w3(worker)
 ;

 sleep(2); //enough time for all channels being registered

 INFO(VARVALS(wordDispatch.channel_senders_reference_count
             ,wordDispatch.channel_receivers_reference_count
             ,hashCollect.channel_senders_reference_count
             ,hashCollect.channel_receivers_reference_count
      )
 );

 collector();
