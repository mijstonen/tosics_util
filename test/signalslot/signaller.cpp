#!/usr/bin/env cpx
#|


state_t sum_collector( int* _result_, int const& _fromslot )
{
    ( *_result_ )+= _fromslot;
    return State(0);
}




namespace tosics::util {

state_t do_not_collect( int*, int const& )
{
    return State(0);
}

    template
    <   typename Return_T
    ,   typename...Args_P
    >
    class
signal
: std::unordered_set<Return_T (*)(Args_P...)>
{
  public:
    using function_pointer_type= Return_T (*)(Args_P...);
    using function_set_type = std::unordered_set<function_pointer_type>;
    using slot_id_type= typename function_set_type::const_iterator;
    constexpr static slot_id_type* DROP_SLOT_ID= nullptr;


        state_t
    connect_func( slot_id_type* slotlocation_, function_pointer_type _fp )
    {
        ASSERT( _fp );  // function pointer needs to be valid, it will not be checked before use
        pair<slot_id_type,bool> result = this->emplace( _fp);
        if ( slotlocation_ ) {
            *slotlocation_ = result.first;
        }
        // 1 indicates that a attempt was made to connect the same function more then once.
        return State( result.second ? 0 : 1);
    }

#if 1
        template <typename Object_T>
        int
    connect_member( slot_id_type* slotlocation_,Object_T *_instance_, Return_T (Object_T::*_method)(Args_P...) )
    {
        return
            connect_func(
                    slotlocation_, _instance_->*_method
                );
    }
#endif

        void
    disconnect(slot_id_type const& _slotlocation)
    {
        this->erase( _slotlocation);
    }

        Return_T
    emit(Args_P&&... _args,
         Return_T const& _initial=0,
         state_t (*_collector)(Return_T*,Return_T const&)=do_not_collect)
    {
        Return_T collect_result{_initial};

        for(auto const& fp: (*this) ) {
            if ( STATEREPORT(
                    _collector( &collect_result, (*fp)( std::forward<Args_P>(_args)...))
                   ,SR_EXCLUDE_0_AND(1)
                   )
            ) {
                // abort loop if returned state_t !=0
                // valid normal terminationis with state_t(1) and that will not be reported by STATEREPORT()
                break;
            }
        }
        return collect_result;
    }
};
}//namespace

int val(string _name, float _val)
{
    INFO(_name,_val);
    return 1;
}

int val2(string _name, float _val)
{
    INFO(_name,_val);
    return 2;
}



int trig(string _name, float _val)
{
    INFO(FUNC_MSG("slot"),VARVALS(_name,_val));
    return 4;
}


struct Foob
{
    int fi=0;

    int fibri(string _name, float _val)
    {
        INFO(VARVALS(fi,_name,_val));
        return 0;
    }
};




#(
    INFO(HGREEN,"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^",RESET);

    using signalsf = tu::signal< int, string,float > ;

    signalsf shoot;

    signalsf::slot_id_type valId, trigId, fibId;

    STATEREPORT( shoot.connect_func( signalsf::DROP_SLOT_ID, val2 ) );
    STATEREPORT( shoot.connect_func( &trigId, trig ) );
    STATEREPORT( shoot.connect_func( &valId, val ) );

    //INFO(VARVAL(shoot.emit("Py ",3.1457,0,sum_collector)));
    INFO(VARVALS(shoot.emit("Py",3.1457,0,
                            [](int*_r_,int const & _s)->state_t
                            {
                                (*_r_)+=_s;
                                return State(0);
                            }
                           )));
    shoot.emit("Michel is ",27);
    INFO();
    shoot.disconnect( valId);
    shoot.emit("Toonen is ",28);
#if 1

    Foob fb;

    shoot.connect_member( signalsf::DROP_SLOT_ID, &fb, &Foob::fibri);
#endif

    INFO(YELLOW,"_________________________________________________________________________",RESET);
#)
