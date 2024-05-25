#!/usr/bin/env cpx
#^<atomic>
#|



// when cpx functionality is used before #( ... # )  you must enter the following line
// This is the most elegant way. You can not add it to #| because if it is not used,
// code will break. Or else introduce a new micro preprocessor char, which should be
// avoided when possible. It turns out that both the filename and #+ are already
// designed to work together.
//
// Eventually cpx-core.cpp could be ommitted from #( . However, that would
// break existing code, demanding all to add #+core.cpp.
//
// Notice that developping prototype code with cpx dependency useually is bad practice.
//  here it is used for experimental research, which is OK.
//  Use of cpx-core.cpp indicates that the code here tests what is usually part of (future) cpx.
//
// Generally required functionality should be moved to the util library
//
#+core.cpp

#include "signalslot.hpp"

#if 1
void proto_testing()
{
    struct Foob
    {
        int fi=0;

            void
        slot_stub(string const&, float)
        {
            INFO(FUNC_MSG("Foob Object_slot_stub"));
        }

            void
        fibri(string  const&_name, float  _val)
        {
            INFO(HYELLOW,"OOOOOOOOO Foob i OOOOOOOOOO",VARVALS(fi,_name,_val),RESET);
        }
            void
        fibrj(string  const&_name, float  _val)
        {
            INFO(HYELLOW,"OOOOOOOOO Foob j OOOOOOOOOO",VARVALS(fi,_name,_val),RESET);
        }
    };

    struct Coof
    {
        int fi=0;

            void
        slot_stub(string  const&, float )
        {
            INFO(FUNC_MSG("Coof Object_slot_stub"));
        }
            void
        blabla(string  const&_name, float  _val)
        {
            INFO(HBLUE,"OOOOOOOOOO Coof  OOOOOOOOO",VARVALS(fi,_name,_val),RESET);
        }
            void
        defered(string  const&_name, float  _val)
        {
            INFO(FUNC_MSG("Put into queue for later processing, so the signal is not lost"));
            INFO(BLUE,"OOOOOOOOOO Coof  OOOOOOOOO",VARVALS(fi,_name,_val),RESET);
        }
    };

        Foob 
    foob;
        Slot<Foob,void (Foob::*)(string const&,float),string const&,float> 
    sf {&foob,&Foob::fibri};
#if 0
    auto sfumf = sf.memfn();
    INFO(VARVAL(sf.memfn_is_defined()));
    sf.memfn_undefine();
    INFO(VARVAL(sf.memfn_is_defined()));
    INFO(VARVAL( sfumf == &Foob::fibri ));
    INFO(VARVAL(sf.memfn_is_defined()));
    INFO(VARVAL( sf.invoke( "Zaterdag", 65)));
#endif
    Signal<string const&,float> sig;

    Coof coof;
    extended_Slot<Coof,void(Coof::*)(string const&,float), string const&,float> dodo{&coof,&Coof::blabla};

    sig.connect( &sf );
    sig.connect( &dodo);


    sig.emit( "Nachtwerk", 221 );

#if 0
    dodo.signal_redirect_to( &Coof::slot_stub,
    [&]() {
         sig.emit("Inactive",1200);
/* private:mutbl_view */         if ( dodo.memfn.mutbl_view == &Coof::slot_stub ) INFO("OKOKOK");
    });
#endif
#if 1

    sig.emit("Again dodo",1212);

    dodo.signal_redirect_to( &Coof::defered,
    [&]() {
         sig.emit("Defering",1238);
    });

    INFO(VARVALS(sizeof(sig),sizeof(dodo)));
#endif

#if 1
    class Generic_Memberfunction_Subclass: public Generic_Memberfunction_base
    {
        // Generic_Memberfunction_base functionality is tested by
        // making mapping to its protected items, fictively make them public
        // hower, this class only exists within its enclosing test function,
        // so prohibbitting abuse of Generic_Memberfunction_base,
        // because its intend that 'user' access is only allowed by
        // typesafe encapsulation with template struct Memberfunction.

            using 
        memfnblock_type = 
            Generic_Memberfunction_base::memfnblock_t;

        Generic_Memberfunction_Subclass() 
        : Generic_Memberfunction_base()
        {}
        
        Generic_Memberfunction_Subclass( Generic_Memberfunction_base::memfnblock_t const& _memberfunc)
        : Generic_Memberfunction_base( _memberfunc)
        {
        }

    
      public:
            static void 
        Test()
        {
            INFO(VARVAL(Generic_Memberfunction_base::GMB_SIZE()));
            memfnblock_type x={1,2,3,4,0xF,6,7,8}, X{'A','S','D','F','G'};
            atomic<Generic_Memberfunction_Subclass> a(x); // requires -latomic
            Generic_Memberfunction_Subclass b(X);

            INFO(VARVAL(a),VARVAL(b));
            INFO(HWHITE,"Swapping a and b values",RESET);
            b = std::atomic_exchange( &a, b );
            INFO(VARVAL(a),VARVAL(b));
        }
    };
    Generic_Memberfunction_Subclass::Test();
#endif 
#if 1
    using FoobMemFn_t = void (Foob::*)(string const&, float);
    atomic< Memberfunction<FoobMemFn_t> >  t{&Foob::fibri};
    Memberfunction<FoobMemFn_t> s{&Foob::fibrj};

    Foob fb;
    (fb.*(t.load().member_function())) ("asd",7.7);
    s = std::atomic_exchange( &t, s);
    (fb.*(t.load().member_function())) ("asd",7.7);
#endif
}
#endif

struct Schema
{
    struct Producer
    {
        Signal<int> sig_num;
    };

    struct Consumer
    {
        Slot<Consumer,void (Consumer::*)(int) const noexcept, int> slot_int;

            void
        display(int _num) const noexcept
        {
            INFO(FUNC_MSG(""), VARVAL(_num));
        }

            void
        blueline() const
        {
            cpx::ScopeDecoratorLines::Line(BLUE);
        }

            void
        redline() const
        {
            cpx::ScopeDecoratorLines::Line(RED);
        }

        Consumer()
        : slot_int( this, & Consumer::display)
        {
        }
    };

    Producer P;
    Consumer C, C2;
    extended_Slot<Consumer,void(Consumer::*)() const> x;
    Signal<> s;

    Schema()
    : x( &C, & Consumer::redline)
    {
        C.slot_int.observe( & P.sig_num );
        x.observe( &s);
    }
};

struct Schema_test : Schema
{
    void test1()
    {
        P.sig_num.emit(5);
        s.emit();
        INFO(VARVAL(x.is_triggering( C, & Consumer::blueline)));
#if 0
/*private*/        x.memfn.const_view = & Consumer::blueline;
#endif
        INFO(VARVAL(x.is_triggering( C, & Consumer::blueline)));
        s.emit();
        INFO(VARVAL(x.is_triggering( C, & Consumer::redline)));

        x.signal_redirect_to( & Consumer::redline, [&](){
            s.emit();
            INFO(VARVAL(x.is_triggering( C, & Consumer::redline)));
        });

        INFO(VARVAL(x.is_triggering( C, & Consumer::redline)));
        INFO(VARVAL(x.is_triggering( C, & Consumer::blueline)));
    }
};

/***
RoundTripComponent illustrates the magic of round tripping signals witout causing recursion and ever growing stacks.

           _________
          |         |
   in-->--| <10? +1 |-->--out
     |    |_________|     |
     |____________________|
                   
                   
If [ <10? +1 ] would be executed inside the increase() immediately  the increase() method calls would (among other needed cals in between)
pile up on the stack. But instead we defer the operation with a lambda and put it on the defered_method_calls dmc that acts as a fifo queue.

So one lambda action is put on the stack and when it becomes active it is aleady removed from the queue but its action is to placeit back on the queue.
And the queue is only flushed after no action is put on the queue again..

Note I do not know (after some years reverse engineering the code again) whether the redirect is a neccessity or just evolution of tests.

This looks silly, but it becomes much more interesting if there are multiple things to do (say searching a data tree).
Instead of heaving exploding thestack by all the recursions (not considiring the correct ness of it anyway) we pile up
the defered actions while those completed are vaished already.

It's not perfect, the signal handlers all have to have a identical signature. 
***/

struct RoudTripperSchema
{
    struct RoundTripComponent
    {
        defered_method_calls<int> dmc;

        extended_Slot<RoundTripComponent, void (RoundTripComponent::*)(int), int> in;
        Signal<int> out;

        RoundTripComponent()
        : in( this, & RoundTripComponent::increase_signals_handler )
        , out()
        {
            in.observe( & out);
        }

        void redirect(int _in)
        {
            dmc.defer(_in);
        }

        void increase(int _in)
        {
            in.signal_redirect_to( & RoundTripComponent::redirect, [&] ()  {
                INFO(VARVAL(_in));
                if ( _in<10 ) {
                    out.emit(_in+1);
                }
            });
        }

        void increase_signals_handler(int _in)
        {
            increase( _in);
            dmc.flush_defered_to( this, & RoundTripComponent::increase );
        }

    };

    RoundTripComponent rtc;

    void run()
    {
        rtc.out.emit(0);
    }
};

struct ExtendedSignalSchema
{
    using event_type=extended_Signal_events<std::string>::watcher_event;

    struct SlotComponent
    {
            void
        message(std::string _msg)
        {
            INFO(VARVAL(_msg));
        }

        Slot<SlotComponent, void(SlotComponent::*)(std::string), std::string> msgSlot;

        SlotComponent()
        : msgSlot( this, & SlotComponent::message )
        {
        }
    };
        void
    show( event_type _event)
    {
        switch( _event)
        {
            case event_type::pre_slots_invoke:
                INFO("Before invoking component methods by connected slots");
                break;
            case event_type::post_slots_invoke:
                INFO("After invoking component methods by connected slots");
                break;
            default:
                // all other events
                INFO(VARVAL(static_cast<int>(_event)));
        }
    }

    SlotComponent tsc1,tsc2;
    extended_Signal<std::string> xSigStr;
    fixated_Signal< 2, event_type> xsEventsDispather;
    Slot<ExtendedSignalSchema, void(ExtendedSignalSchema::*)(event_type),event_type> xsEventsListner, xsEventsLis2  /*, xsE3*/;

    ExtendedSignalSchema()
    : xsEventsListner( this, & ExtendedSignalSchema::show )
    , xsEventsLis2( this, & ExtendedSignalSchema::show )
    //, xsE3( this, & ExtendedSignalSchema::show )
    {
#if 0
        xsEventsDispather.connect( { & xsEventsListner, & xsEventsLis2 /*, & xsE3 */ });
        xSigStr.watcher( & xsEventsDispather);
        xSigStr.connect( { & tsc1.msgSlot, & tsc2.msgSlot });
#else
        xsEventsDispather.connect( & xsEventsListner);
        xsEventsDispather.connect( & xsEventsLis2);
        xSigStr.watcher( & xsEventsDispather);
        xSigStr.connect( & tsc1.msgSlot);
        xSigStr.connect( & tsc2.msgSlot);
#endif
        INFO(VARVAL(xsEventsDispather.hasFreeLocation()));
        xsEventsDispather.disconnect( & xsEventsListner);
        INFO(VARVAL(xsEventsDispather.hasFreeLocation()));
     }

    ~ExtendedSignalSchema()
    {
#if 0        
        xSigStr.disconnect( { & tsc2.msgSlot, & tsc1.msgSlot } );
#else
        xSigStr.disconnect( & tsc2.msgSlot );
        xSigStr.disconnect( & tsc1.msgSlot );
#endif        
    }

        void
    run()
    {
        using namespace std::literals::string_literals;
        xSigStr.emit("Hello to slots"s);
    }
};

struct ObserveSchema
{
    void show(int _n)
    {
        INFO(VARVAL(_n));
    }


    Signal<int> ns;
    fixated_Signal<1, int> fs;
    extended_Signal<int> xs;

    extended_Slot<ObserveSchema,void(ObserveSchema::*)(int),int> in;

    ObserveSchema()
    : in{ this, & ObserveSchema::show}
    {
        in
          .observe( & ns)
          .observe( & fs)
          .observe( & xs)
        ;
    }

    ~ObserveSchema()
    {
        /*
         The signal types keep track about the connected slots to which to emit(), by the pointers to the slots.
         For that reason the signal types need to be informed when a slot is discarded from memory (usually by
         going out of scope of the owning/enclosing component object). So it is the resonsibility of the
         component to remove the pointer to the discarded slot out of each slot pointer collection of each signal
         type. Failing to do so may cause a invoke call to a dangeling slot pointer when the signal is emitted.
         This causes undefined behaviour, probably a program crash in the best case.

         Remove the connection in reverse order of creation to be stack and memory management friendly.
         */

        in
          .un_observe( & xs)
          .un_observe( & fs)
          .un_observe( & ns)
        ;
    }

    void
    run()
    {
        ns.emit(1);
        fs.emit(2);
        xs.emit(3);
    }
};

struct AccumulationSchema
{
    fixated_Signal< 3, int* > collector;
    void provide( int* out_ )
    {
        *out_ += 1;
    }

    Slot< AccumulationSchema,void(AccumulationSchema::*)(int*),int*> a, b, c;

    AccumulationSchema()
    : a( this, & AccumulationSchema::provide )
    , b( this, & AccumulationSchema::provide )
    , c( this, & AccumulationSchema::provide )
    {
#if 0        
        collector.connect( { &a, &b, &c } );
#else        
        collector.connect( &a );
        collector.connect( &b );
        collector.connect( &c );
#endif        
    }

    ~AccumulationSchema()
    {
#if 0        
        collector.disconnect( { &c, &b, &a } );
#else
        collector.disconnect( &c );
        collector.disconnect( &b );
        collector.disconnect( &a );
#endif
    }

    void
    run()
    {
        int box=0;
        collector.emit( & box );
        INFO(VARVAL(box));
    }
};



void schema_tests()
{
    Schema_test().test1();
    ExtendedSignalSchema();
    ObserveSchema().run();
    RoudTripperSchema().run();
    AccumulationSchema().run();
}

#(
    WITH_SCOPE_DECORATOR_LINES;
    proto_testing();
    schema_tests();
    (void)"3";
    //TODO:
    // Test lazy_register lazy_unregister is_registered_at
#)
