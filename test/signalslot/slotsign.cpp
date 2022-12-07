#!/usr/bin/env cpx
#|

namespace tosics::util
{
        class
    signal_interface;

        template<typename... Types_P>
        class
    slot_interface
    {
      protected:
#if 0
          struct {
              unsigned enabled:1;
          }
          control_flags
#endif
        slot_interface() = default;
      public:
        virtual void listen( signal_interface* ) = 0;
        virtual void activate(Types_P... _instances_)=0;
    };
        template<typename... Args_P>
        class
    void_function_slot
    : public slot_interface<Args_P...>
    {
        using func_ptr_type = void(*)(Args_P...);

        func_ptr_type m_pfunc;

      public:
        void_function_slot(func_ptr_type _pfunc)
        : m_pfunc(_pfunc)
        {
        }
          void
        activate(Args_P&&..._args)
        {
            (*m_pfunc)( std::forward<Args_P>(_args)...);
        }
    };

        template
        <   typename Return_T
        ,   typename... Args_P
        >
        class
    returning_function_slot
    : public slot_interface<Return_T,Args_P...>
    {
        using func_ptr_type = Return_T(*)(Args_P...);

        func_ptr_type m_pfunc;

      public:
        returning_function_slot(func_ptr_type _pfunc)
        : m_pfunc(_pfunc)
        {
        }
          void
        activate(Return_T *returning_,  Args_P&&..._args)
        {
            ( *returning_ )= (*m_pfunc)( std::forward<Args_P>(_args)...);
        }
    };
        template<typename Object_T, typename... Args_P>
        class
    void_member_slot
    : public slot_interface<Object_T*,Args_P...>
    {
        using instance_ptr_type = Object_T*;
        using mem_ptr_type = void(Object_T::*)(Args_P...);

        instance_ptr_type m_instance_ptr;
        mem_ptr_type      m_mbr_ptr;

      public:
        void_member_slot(instance_ptr_type _instance_ptr,mem_ptr_type _mbr_ptr)
        : m_instance_ptr(_instance_ptr), m_mbr_ptr(_mbr_ptr)
        {
        }
          void
        activate(Args_P&&..._args)
        {
            (m_instance_ptr->*m_mbr_ptr)( std::forward<Args_P>(_args)...);
        }
    };

}//namespace tosics::util





#(
    INFO(HGREEN,"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^",RESET);
    INFO(VARVALS(sizeof(void_function_slot<char>),sizeof(returning_function_slot<int,int>)));

    INFO(YELLOW,"_________________________________________________________________________",RESET);
#)
