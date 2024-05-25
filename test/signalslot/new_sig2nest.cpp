#!/usr/bin/env cpx
#|

// when cpx functionality is used before #( ... # )  you must enter the following line
// This is the most elegant way. You can not add it to #| because if it is not used,
// code will break. Or else intreduce a new micro preprocessor char, which should be
// avoided when possible. It turns out that both the filename and #+ are already
// designed to work together.
//
// Eventually cpx-core.cpp could be ommitted from #( . However, that would
// break existing code, demanding all to add #+core.cpp.
//
// Notice that developping prototype code with cpx dependency useually is bad practice.
//  here it is used for experimental research, which is OK.
//
// Generally required functionality should be moved to the util library
//


#+core.cpp



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
        INFO(HYELLOW,"OOOOOOOOO Foob OOOOOOOOOO",VARVALS(fi,_name,_val),RESET);
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


#if 1
void proto_testing()
{
    Foob foob;
    Slot<Foob,string const&,float> sf{&foob,&Foob::fibri};
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
    extended_Slot<Coof, string const&,float> dodo{&coof,&Coof::blabla};

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
        Slot<Consumer,int> slot_int;

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
    extended_Slot<Consumer> x;
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
//Schema_test().test1();


struct RoudTripperSchema
{

    struct RoundTripComponent
    {
        defered_method_calls<int> dmc;

        extended_Slot<RoundTripComponent,int> in;
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

        Slot<SlotComponent,std::string> msgSlot;

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
    Signal<event_type> xsEventsDispather;
    Slot<ExtendedSignalSchema, event_type> xsEventsListner, xsEventsListner2;

    ExtendedSignalSchema()
    : xsEventsListner( this, & ExtendedSignalSchema::show )
    , xsEventsListner2( this, & ExtendedSignalSchema::show )
    {

        xsEventsDispather.connectS( xsEventsListner, xsEventsListner2);

        xSigStr.watcher( & xsEventsDispather);

        tsc1.msgSlot.observe( & xSigStr);
        tsc2.msgSlot.observe( & xSigStr);
        xsEventsDispather.disconnectS( xsEventsListner2, xsEventsListner);
    }

    ~ExtendedSignalSchema()
    {
        tsc2.msgSlot.un_observe( & xSigStr);
       tsc1.msgSlot.un_observe( & xSigStr);
    }

        void
    run()
    {
//        using namespace std::literals::string_literals;
        xSigStr.emit("Hello to slots");
    }
};

void schema_tests()
{
    ExtendedSignalSchema().run();
}

#(
    WITH_SCOPE_DECORATOR_LINES;
    //proto_testing();

    schema_tests();


    (void)"7p;wergt ;";
    //TODO:
    // Test lazy_register lazy_unregister is_registered_at
#)
