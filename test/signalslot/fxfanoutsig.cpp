#!/usr/bin/env cpx
#|

    template<
        typename T,
        std::size_t N
    > struct
fixed_pointer_buffer
: std::array<T*,N>
{
    std::size_t next=0;

    /*
     * add a pointer
     * assign a pointer on index
     * remove a pointer by its own value
     * remove a pointer by index
     * make iterator skip cleared pointers
     * count valid pointers
     * search for a pointer
     */

};


    template<size_t _MaxFanOut, typename ... Args_P> class
fixed_fanout_Signal
: public slot_base<Args_P...>, std::array< slot_base<Args_P...>::slot_pointer_type
{
    /*
     * is_connected_to  // searches the slot pointer in the pointer buffer, skipping nullptr's
     * connect    // if not is_connected_to()  adds pointer
     * disconnect // if is_connected_to() erase from pointer buffer
     * emit       // for each non nullptr (aka: valid) slot pointer call its invoke()
     * invoke     // call emit()

};

#(
#)
