#pragma once

/* must have #include <atomic> before, not here */

namespace tosics::util {

    //TODO: change namespace to tosics :: udss
    //      [udss] stands for: unidirectional signals and slots


        template<typename Outer_T>using
    slot_is_nested_of = nested_of16_t<Outer_T>; // a slot should be nested in the consumer class definition and be a member of a instance of that class.
                                                // take the appropiate nested_of class if the offset to the parent object exceeds the limitations.
                                                // The digit part denotes number of bits for the available for the member tho this offset.
        template<typename ... Args_P> class
    Signal;
        template<typename ... Args_P> class
    extended_Signal;
        template<std::size_t NumberOfSlots, typename... Args_P> class
    fixated_Signal;
//______________________________________________________________________________________________________________________
    template<typename ... Args_P> class
slot_base
{
  public:
    virtual void invoke(Args_P..._args) const = 0;
    using slot_pointer_type= slot_base<Args_P...> const*;
    using slot_pointer_set_type= std::unordered_set<slot_pointer_type >;

    auto& observe( Signal<Args_P...>* psig_) const                              { psig_->connect(this); return *this; }
    auto& un_observe( Signal<Args_P...>* psig_) const                     { psig_->disconnect( this); ; return *this; }
    auto is_observing( Signal<Args_P...> const* _psig) const                  { return _psig->is_connected_to( this); }

    auto& observe( extended_Signal<Args_P...>* pxsig_) const                   { pxsig_->connect(this); return *this; }
    auto& un_observe( extended_Signal<Args_P...>* pxsig_) const            { pxsig_->disconnect( this); return *this; }
    auto is_observing( extended_Signal<Args_P...> const* _pxsig) const       { return _pxsig->is_connected_to( this); }

        template<std::size_t NumberOfSlots> auto&
    observe( fixated_Signal<NumberOfSlots,Args_P...>* pfsig_)            const { pfsig_->connect(this); return *this; }
        template<std::size_t NumberOfSlots> auto&
    un_observe( fixated_Signal<NumberOfSlots,Args_P...>* pfsig_)      const { pfsig_->disconnect( this);return *this; }
        template<std::size_t NumberOfSlots> auto
    is_observing( fixated_Signal<NumberOfSlots,Args_P...> const* _pfsig) const { return _pfsig->is_connected_to(this);}
};

//______________________________________________________________________________________________________________________

    template<typename ... Args_P> class
Signal
: public slot_base<Args_P...>, slot_base<Args_P...>::slot_pointer_set_type
{

  public:

    using typename slot_base<Args_P...>::slot_pointer_set_type;
    using typename slot_base<Args_P...>::slot_pointer_type;

        state_t
    connect( slot_pointer_type _slotPtr )
    {
        bool is_inserted(this->slot_pointer_set_type::emplace( _slotPtr).second);
        return State( is_inserted ?0 :1 );
    }

        bool
    is_connected_to( slot_pointer_type _slotPtr ) const
    {
        return ( ( this->slot_pointer_set_type::find( _slotPtr) )!= slot_pointer_set_type::end() );
    }

        state_t
    disconnect( slot_pointer_type _slotPtr )        // returns 1 if _slotPtr was not in set and so not disconnected
    {
        return State( this->slot_pointer_set_type::erase( _slotPtr) ?0 :1 );
    }

        void
    emit(Args_P... _args) const
    {
        for(auto slot_ptr: (*this) ) slot_ptr->invoke( _args...);
    }

        void
    invoke(Args_P..._args) const override
    {
        return emit(_args...);
    }

};
//______________________________________________________________________________________________________________________

    struct
Generic_Memberfunction_base  // must adhere to conept: TriviallyCopyable
{
  protected:
    class   Undefined;
    typedef void (Undefined::*a_memberfunc_type)();
    typedef std::uint8_t  memfnblock_t[sizeof(a_memberfunc_type)];

    static_assert( std::is_trivially_copyable<a_memberfunc_type>());
    static_assert( std::is_trivially_copyable<memfnblock_t>() );
    constexpr static std::size_t GMB_SIZE() { return sizeof(memfnblock_t); }


    memfnblock_t m_memfnblock;

    Generic_Memberfunction_base()  // required by std::atomic_exchange
    {
       memset(m_memfnblock,0,GMB_SIZE());
    }

    Generic_Memberfunction_base(memfnblock_t const&  _memfnblock)
    {
        memcpy( &m_memfnblock[0],&_memfnblock[0], GMB_SIZE());
    }
        void
    swap_with( memfnblock_t* _alternative_member_function_)
    {
        memfnblock_t tmp;
        //tmp= m_memfnblock;
        memcpy( &tmp[0], &m_memfnblock[0], GMB_SIZE());
        //m_memfnblock=_alternative_member_function_;
        memcpy( &m_memfnblock[0],&_alternative_member_function_[0], GMB_SIZE());
        //_alternative_member_function_= tmp;
        memcpy( &_alternative_member_function_[0], &tmp[0], GMB_SIZE());
    }

        bool
    is_equal_to(memfnblock_t const&  _memfnblock) const
    {
        return !bcmp( &m_memfnblock[0], &_memfnblock[0], GMB_SIZE());
    }


  public:
        void
    to_os(std::ostream* pOs_) const             // for print debugging, may be removed or changed
    {
        uint8_t const* first{m_memfnblock};
        uint8_t const* last{first+GMB_SIZE()};
        ( *pOs_ )<<std::hex;
        for(;;) {
            ( *pOs_ )<< "0x"<< static_cast<uint16_t>(*first);
            if ( ++first==last ) {
                break;
            }
            ( *pOs_ )<<',';
        }
        ( *pOs_ )<<std::dec;
    }

};
// for print debugging
inline std::ostream& operator << (std::ostream & os_, Generic_Memberfunction_base const& _m) { _m.to_os( &os_); return os_; }

    template<typename MemberFunction_T> class
Memberfunction       // must adhere to conept: TriviallyCopyable, because of possible atomic use.
: private Generic_Memberfunction_base
{
    // Memberfunction is a type safety wrapper for Generic_Memberfunction_base, you can only get
    // the type out what was put in. Memberfunction never shall have its own data members
    static_assert( sizeof(Generic_Memberfunction_base)==sizeof(Generic_Memberfunction_base::memfnblock_t));

        bool
    is_initialized() const
    {
        Memberfunction empty;
        bool returnValue { (*this) != empty };
        return returnValue;
    }

  public:

    Memberfunction()
    : Generic_Memberfunction_base()
    {
    }

    Memberfunction( MemberFunction_T const& _memberfunc)
    : Generic_Memberfunction_base( reinterpret_cast<memfnblock_t const&>(_memberfunc))
    {
    }
        MemberFunction_T const&
    member_function() const
    {
        return  *reinterpret_cast<MemberFunction_T const*>( &m_memfnblock );
    }
        void
    swap_with( MemberFunction_T *_alternative_member_function_)
    {
        this->Generic_Memberfunction_base::swap_with( reinterpret_cast<memfnblock_t*>(_alternative_member_function_));
    }
        bool
    is_equal_to( MemberFunction_T const& _other_member_function) const
    {
        return this->Generic_Memberfunction_base::is_equal_to(
            reinterpret_cast<memfnblock_t const&>(_other_member_function));
    }
        bool
    operator ==( Memberfunction const& _other) const
    {
        return this->is_equal_to( _other.member_function());
    }

    // Allow a Memberfunction to be tested, to truly carry a member function.
        operator
    bool() const
    {
        return is_initialized();
    }
};


    template<typename Object_T, typename MemberFunction_T, typename... Args_P> class
Slot
:   public slot_base<Args_P...>
,   protected slot_is_nested_of<Object_T>
{
    static_assert( sizeof(Memberfunction<MemberFunction_T>)==sizeof(Generic_Memberfunction_base));
    static_assert( std::is_trivially_copyable< Memberfunction<MemberFunction_T> >::value );

protected:
       Memberfunction<MemberFunction_T> m_memfn{};


  public:
    Slot() = default;

    Slot(Object_T* _pObject_, MemberFunction_T _memfn)
    : slot_base<Args_P...>(), slot_is_nested_of<Object_T>(_pObject_), m_memfn(_memfn)
    {}

    void invoke(Args_P..._args) const override
    {
        (this->mutable_outer()->*(m_memfn.member_function())) ( _args...);
    }

};// Slot

/*
The following is (besides others) to support round trip cases. Not by putting functionality in the basic types,
but fine grained subclassing functionality and extening of the basic types.
In round trip cases, indefinite recursion could ocure.

extended_Slot
-------------
This is avoided by the generic functionality to scope specific redirection of of the signal. In words:
"If a signal is processed by a method, first redirect the signal to another method".
Its up to that method to decide what to do. It could ignore the incomming signal, count
them or whatever, delegate it to another object.
Avoiding roundtrip recursion is done by storing the arguments in a special buffer type with the name:

defered_method_calls
--------------------
We want redirection to co-opperate with defering the signal processing to a later stage.
To do that the signal arguments need to be stored in a tuple. And for multiple times the signal is received,
each such tuple should be buffered, in (chronologic) sequential order.

A extended slot will redirect the signal to the defer method of the defered_method_calls object. When
leaving redirect scope, the enclosing function will flush the arguments to a specified method. When
that method is the original method, the process is repeated because while handling the arguments of the
defered signal, any emitted signal is defered and buffered.
In a signle threaded situation, this case occures when the signal handler it self (or one of its callee's)
emits the signal. In the multithreaded scenario, other signals might be redirected while in the signal handler.
However, mutal exclusion correctness is not the resposibility of udss, but of the code that uses udss (aka the
component classes that emit signals and have slots to handle them).
Test scenario's only cover single threaded scenario's.

Notice that udss does only a little, the controlling code has a great resonsibility to use udds classes correctly,
it is literally "in control". For example it can use extended_Slot::signal_redirect_to_if( _conditional, ....) to
relax the prohibbition of recursion by maintaining a recurion counter and only redirect if that reaches a limit.
That recurion relaxing might be usefull to to less (costly) buffering, intersignal lockings e.g. .

    signal_redirect_to_if( this->recursions > MAX_RECURSIONS ), & controller::redirecting, [&](){
        ++ this->recursions;
        .....
        output.emit(data1);
        ....
        output.emit(data2);
        ....
    });


fixed_fanout_Signal
-------------------
This is a rather limited, but conceptionally very usefull type. Often signal slot connections are limited 1
signal to only a few (say  0..9) connected. The normal signal class has to store the slot pointers in a rather
complicated data structure, it uses a unordered_set. This uses heap memory and has certain overhead that
becomes a burden when only one slot or a few slots are connected. In many cases (for example in a schema class
with limited and fixed number of slots and signals) the number of connected slots is easily counted.
The buffer for the slot pointers is allocated internally and can not change. But you can - within the range of
0.._MaxFanOut-1 place slot pointers in the buffer and also remove them. Compared to Signal, fixed_fanout_Signal
is much much faster. It does never need (slow) heap memory allocations. The number of address (pointer)
indirections is minimal and a emit basically only iterates over a small chunk of local (stack) memory to
access the slots.

extended_Signal
---------------
For signal chaining, a extended_Signal it self emits signals with a int as a number to indicate the kind of event.
This is used to couple activities to events, but without passing the arguments. Notice that Signal an extended_Signal
have the same methods but have no (inheritance) relation.

passive_slot and optional_watcher
---------------------------------
Are used for helping the the extended_Signal mechanism, but are them self providers of shallow functionallity that
could be used for other purposes.
*/


//______________________________________________________________________________________________________________________

    template<typename ... Args_P> class
passive_slot
: public slot_base<Args_P...>
{
public:
    void invoke(Args_P.../*unused arguments*/) const override final
    {
        /*invoke deliberately does nothing*/
        /*
         A passive_slot acts as a (active) slot place holder.
         replacing activity with inactivity. This way the
         virtual function mechanism is used to do where
         otherwhise a costly condition (aka checking for nullptr)
         would be required
        */
    };

    constexpr passive_slot(){};

    static auto TheDummyAddr() { static const passive_slot<Args_P...>  Dummy; return &Dummy; }
};

    template <typename WatcherEvent_T>
class optional_watcher
{
  public:
    using watcher_event_type = WatcherEvent_T;
    using watcher_slot_type = slot_base<watcher_event_type>;

  private:
    watcher_slot_type const* m_watcher;

  public:
     static watcher_slot_type const* TheDummyAddr() { return passive_slot<watcher_event_type>::TheDummyAddr(); }

      // setter and getter for watcher
    void watcher( watcher_slot_type const* _watcher) { m_watcher= _watcher; }
    watcher_slot_type const* watcher() const { return m_watcher; }
    void watcher_stop() { watcher(TheDummyAddr()); }

    optional_watcher()
    : m_watcher{TheDummyAddr()}
    {
    }

    ~optional_watcher()
    {
        watcher_stop();
    }
};

    template<typename ... Args_P> class
extended_Signal_events
{
  public:
    enum class watcher_event : int
    {   Null
    ,   do_connect_slot
    ,   pre_slots_invoke
    ,   post_slots_invoke
    ,   invoke_emit
    ,   is_connected_slot
    ,   is_not_connected_slot
    ,   do_disconnect_slot
    };
};
    template<typename ... Args_P> class
extended_Signal
: public extended_Signal_events<Args_P...>
, public slot_base<Args_P...>
, public optional_watcher<typename extended_Signal_events<Args_P...>::watcher_event>
, slot_base<Args_P...>::slot_pointer_set_type
{
    using typename extended_Signal_events<Args_P...>::watcher_event;
    using typename slot_base<Args_P...>::slot_pointer_set_type;
    using typename slot_base<Args_P...>::slot_pointer_type;

  public:
        state_t
    connect( slot_pointer_type _slotPtr )
    {
        this->f()->invoke( watcher_event::do_connect_slot);
        bool is_inserted(this->slot_pointer_set_type::emplace( _slotPtr).second);
        return State( is_inserted ?0 :1 );
    }
        bool
    is_connected_to( slot_pointer_type _slotPtr ) const
    {
        if ( ( this->slot_pointer_set_type::find( _slotPtr) )== slot_pointer_set_type::end() ) {
            this->watcher()->invoke( watcher_event::is_not_connected);
            return false;
        }

        this->watcher()->invoke( watcher_event::is_connected_slot);
        return true;
    }
        state_t
    disconnect( slot_pointer_type _slotPtr )
    {
        this->watcher()->invoke( watcher_event::do_disconnect_slot);
        return State( this->slot_pointer_set_type::erase( _slotPtr) ?0 :1 );
    }
        void
    emit(Args_P... _args) const
    {
        this->watcher()->invoke( watcher_event::pre_slots_invoke);
        for(auto slot_ptr: (*this) ) slot_ptr->invoke( _args...);
        this->watcher()->invoke( watcher_event::post_slots_invoke);
    }
        void
    invoke(Args_P..._args) const override
    {
        this->watcher()->invoke( watcher_event::invoke_emit);
        return emit(_args...);
    }

#if 0
    void connect( std::initializer_list<slot_pointer_type> _slotPtrs )
    {
        for( auto s: _slotPtrs) { connect( s); }
    }

    state_t disconnect( std::initializer_list<slot_pointer_type> _slotPtrs)
    {
        auto no_disconnetcts= State(0);
        for( auto s: _slotPtrs) {
            no_disconnetcts += disconnect( s);
        }
        return State(no_disconnetcts);
    }
#endif
};

    template<std::size_t NumberOfSlots, typename ... Args_P> class
fixated_Signal
: public slot_base<Args_P...>
, std::array< typename slot_base<Args_P...>::slot_pointer_type,NumberOfSlots>
{
    using typename slot_base<Args_P...>::slot_pointer_type;
/*
 Instead of not initializing or initializing with nullptr, all array positions are initialized with a
 passive slot. Notice that usually all array positions are ment to be filled with 'active' slots,
 so emit works without any containter indirections (as is the case with othwr std 'set' containers
 making  this emit fast and with mimimal overhead. A array location wich holds the dummy is considered
 empty and can be replaced with a 'active' slot pointer. That is done by connect(), where as disconnect()
 replaces the 'active' slot pointer with dummy.
 Notice no order is maintained and no 'free position' index is required. NumberOfSlots is considered small
 (say less then 10 although there is practically no limit).
 In contrary to other signal types, the number of free locations is <= NumberOfSlots
 When all free locations are used, failing to connect (in RELEASE mode) will be ignored. In debug mode
 it is handled with ASSERT. Notice that connect() should have the same signature as the other signal types,
 hence return a result value (aka: connect succeeded or failed) is not an option.
 There is a way out of this by: if ( hasFreeLocation() ) connect( slotPtr); else .... . However it is
 considered used rarely. Possibly it would already be better to use another signal type in dynamic
 use cases.
*/

  public:
    fixated_Signal()
    {
        auto free=passive_slot<Args_P...>::TheDummyAddr();
        for( auto& e: *this ) e = free;
    }


  protected:
        slot_pointer_type const*
    match_location( slot_pointer_type _slotPtr ) const
    {
        for( auto const& e : *this ) if ( e == _slotPtr ) return &e;
        return nullptr; // no matching location found
    }

        slot_pointer_type*
    modifiable_match_location( slot_pointer_type _slotPtr)
    {
        return const_cast<slot_pointer_type*>( match_location( _slotPtr));
    }

  public:
        bool
    hasFreeLocation() const
    {
        return is_connected_to( passive_slot<Args_P...>::TheDummyAddr());
    }

        state_t
    connect( slot_pointer_type _slotPtr )  // single pass through array algorithm
    {
        slot_pointer_type* itr {this->begin()};
        slot_pointer_type* end {this->end()};
        slot_pointer_type free = passive_slot<Args_P...>::TheDummyAddr();
        state_t returnValue= -1; // connection is not (jet) made
        while ( itr != end ) {
            if ( *itr == _slotPtr ) {
                return State(1); // slot was already connected to signal
            }
            if ( *itr==free ) {
                *itr = _slotPtr;
                returnValue= 0;
                break;
            }
            ++itr;
        }
        if ( returnValue ) {
            //  here itr == end ERROR: array is full _slotPtr is NOT connected
            return State(-1);
        }
        while ( ++itr != end ) {
            // free any other location that might already have been assigned to _slotPtr
            // because this is a set and _slotPtr should be in only once
            if ( *itr == _slotPtr ) {
                *itr= free;
                ++returnValue;
            }
        }
        // 0 (perfect, jus made connection)
        // notification: 1 (could be, strange reconnect, but OK)
        // notification: >1 (should not happen, but OK, it self healing because multiple locations where free'd)
        return State( std::move(returnValue));
    }

        bool
    is_connected_to( slot_pointer_type _slotPtr ) const
    {
        auto pml=match_location( _slotPtr);
        return pml ?true :false;
    }
        state_t
    disconnect( slot_pointer_type _slotPtr )
    {
        auto pml= modifiable_match_location( _slotPtr);
        if ( pml ) *pml = passive_slot<Args_P...>::TheDummyAddr();
        return State( pml ?0 :1 );
    }

        void
    emit(Args_P... _args) const
    {
        for(auto slot_ptr: (*this) ) slot_ptr->invoke( _args...);
    }
        void
    invoke(Args_P..._args) const override
    {
        return emit(_args...);
    }

};
//______________________________________________________________________________________________________________________

    template<typename Object_T, typename MemberFuncion_T, typename... Args_P> class
extended_Slot : public Slot<Object_T,MemberFuncion_T,Args_P...>
{
  public:
    using Slot<Object_T,MemberFuncion_T,Args_P...>::Slot; // constructor from base class

        bool
    is_triggering( Object_T const& _object, MemberFuncion_T _memfn )
        noexcept
    {
        if ( &_object != this->outer() ) {
            return false;
        }
        return ( this->m_memfn.is_equal_to( _memfn) );
    }
        template <typename Lambda_T> auto
    signal_redirect_to( MemberFuncion_T _alternative, Lambda_T _actions_while_redirected)
    {
        this->m_memfn.swap_with( & _alternative);
        try {
            _actions_while_redirected();
        }
        catch ( ... ) {
            this->m_memfn.swap_with( & _alternative);
            throw;
        }
        this->m_memfn.swap_with( & _alternative);
    }
#endif
        template <typename Lambda_T> auto
    signal_redirect_to_if( bool _condition, MemberFuncion_T _alternative, Lambda_T _lambda)
    {
        if ( _condition ) return signal_redirect_to( _alternative, _lambda);
        /*else*/          return _lambda;
    }
};

namespace {
    template <typename Object_T, typename memfn_T, typename tuple_T, size_t... _I_> inline void
memfn_apply_get_each_arg_by_index_sequence( Object_T* _p_, memfn_T _f, tuple_T && _t,std::index_sequence<_I_...>)
{
    (_p_->*_f)(std::get<_I_>(std::forward<tuple_T>(_t))...);
}
}//namepace
    template <typename Object_T, typename memfn_T, typename... Args_P> void
memfn_apply(Object_T* _p_, memfn_T _f, std::tuple<Args_P...> const& _t)  // free function to call (file scope) memfn_apply_get....()
{                                                                            // example: memfn_apply( brithDate, & Date::format, birthDateFormatArgs_tuple)
    memfn_apply_get_each_arg_by_index_sequence( _p_, _f, _t,std::index_sequence_for<Args_P...>{});
}
    template <typename... Args_P> class
defered_method_calls
{
    std::deque< std::tuple< typename std::decay<Args_P>::type... > > m_mtdArgsBuf;

  public:
    void defer( Args_P&&..._args) { m_mtdArgsBuf.emplace_back(std::forward<Args_P>(_args)...); }
    void defer( Args_P const&..._args) { m_mtdArgsBuf.emplace_back(_args...); }

        template <typename Object_T, typename /*void(...)*/ memfn_T> void
    flush_defered_to( Object_T* _p_, memfn_T _f)
    {
        while( !m_mtdArgsBuf.empty() ) {
            auto args_tuple = m_mtdArgsBuf.front();  // need args_tuple to pop_front() before ....
            m_mtdArgsBuf.pop_front();               // memfn_apply_get....() to avoid need of try catch block
            memfn_apply_get_each_arg_by_index_sequence( _p_, _f, std::move( args_tuple), std::index_sequence_for<Args_P...>{});
        }
    }
};
// TODO: multithreaded_method_call_queue   (front and back should be accessed mutal exclusively)

}//namespace tosics::util
