#pragma once
//member.hpp
#ifndef MEMBER_HPP
#  define MEMBER_HPP 1



#ifndef __cpp_lib_byte  // compiler or library depended
# define NEEDS_BYTE_DEFINITION 1
#else
//  the cpp library has this type in cstddef std::byte
# define NEEDS_BYTE_DEFINITION 0
#endif

//:member_of    base class for a member object that stores the offset to its owning object and has owner().
//              Simplifies interaction between nested classes.
//              If the owning (and enclosing) class is copied or moved to an other memory location,
//              the relation remains in tact. Either if a member is copied to another member, the owner<-member
//              relation remains in tact.
//
// NOTE: assumes  addres of owner < addres of member < addres of owner + sizeof owner
// precondition: likely( -_M_offset_to_owner<= PTRDIFF_MAX )
// pedantic: -_M_offset_to_owner< (1<<30)   (fails on abnormal huge (larger then 1G) owner object
// Asserts ommitted because the conditions are extreemly unlikely on 64 bit systems.
//
// About defaults for Offset_T and MAX_OFFSET.
// As multiple members occupy unneccesary bytes for storring the offset for likely owner
// objects being smaller then UINT16_MAX (aka 32KByte), the offset is stored in sizeof(uit16) (aka 2) byte
// offset_type, saving unused memory. Note that the compiler can still align addresses and take up more
// space, but when space optimized, the compiler may take advanatage of it.
// Alternatively change type fror Offset_T and change MAX_OFFSET accordingly.
//
//



namespace tosics::util {

    template<
        typename Owner_T,
        typename Offset_T = uint16_t, ptrdiff_t MAX_OFFSET = UINT16_MAX  //Change Offset_T and UINT16_MAX together
    >
    class
member_of
{
  private:
    constexpr static Offset_T OFFSET_TO_OWNER_UNSET = -1;


#if NEEDS_BYTE_DEFINITION
        enum class
    e_byte
    : unsigned char
    {
    };
    using byte_type = e_byte;
#else
    using byte_type = std::byte;
#endif

        Offset_T
    offset_to_owner(Owner_T* _owner) noexcept
    {
        byte_type* pOwnerByteAdr  { static_cast<byte_type*>(static_cast<void*>(_owner)) };
        byte_type* pMemberByteAdr { static_cast<byte_type*>(static_cast<void*>(this)) };
        ptrdiff_t  return_value   { pMemberByteAdr- pOwnerByteAdr };

        static_assert( sizeof(Owner_T)< MAX_OFFSET,"Practically limited size of Owner_T, otherwise modify template args." );

        ASSERT( return_value>= 0 );//member should be on or abouve lower bound of  its owner
        ASSERT( sizeof(Owner_T)> static_cast<size_t>(return_value) );//member should be below upper bound of its owner
        ASSERT( return_value< MAX_OFFSET ); // casting may not cause loss of digits

        return static_cast<Offset_T>(return_value);
    }

  protected:
    const Offset_T _M_offset_to_owner = OFFSET_TO_OWNER_UNSET;

    member_of() noexcept
    : _M_offset_to_owner(OFFSET_TO_OWNER_UNSET)
    {
    }

    member_of(Owner_T* _owner) noexcept
    : _M_offset_to_owner(offset_to_owner(_owner))
    {
        // read comments above
    }

    // if _M_offset_to_owner is already initialized, it should not be changed by copying, that's incorrect
    member_of(const member_of& _from) noexcept
    : _M_offset_to_owner( _M_offset_to_owner==OFFSET_TO_OWNER_UNSET ? _from._M_offset_to_owner : _M_offset_to_owner)
    {
        ASSERT( _M_offset_to_owner!=_from._M_offset_to_owner );

        // realy ? can't I copy a member from one to another ownwer ?
        // ASSERT( const_cast<member_of&>(_from).owner()==owner() );  // copies are only allowed within the same owner
    }

    member_of& operator = (const member_of& _from) noexcept
    {
        // emplace copy
        new(this) member_of(_from);

        return *this;
    }

    // NOTE: Members are always lvalues, it (probably) does not make sense to move (from) them.
    member_of(member_of&& _from) = delete;//TODO whenever it will be required - being deleted - compilation will warn.
    //
    //: _M_offset_to_owner( _M_offset_to_owner==OFFSET_TO_OWNER_UNSET ? _from._M_offset_to_owner : _M_offset_to_owner)
    //{
    //  // _from._M_offset_to_owner==OFFSET_TO_OWNER_UNSET; //undefinining is a write operation and it defeats the
    //                                                      //purpose of move. So move, still is actually a copy.
    //}

    Owner_T* owner() noexcept
    {
        // this method guarantees not to change this or owner by it self altough,
        // changes are allowed though the returned pointer. Otherwhise it should
        // not be reused in the const version below.

        ASSERT(_M_offset_to_owner<sizeof(Owner_T));
        byte_type*   pOwnerByteAdr{ static_cast<byte_type*>( static_cast<void*>( this))- _M_offset_to_owner };
        return  static_cast<Owner_T*>( static_cast<void*>(  pOwnerByteAdr  ));
    }

    Owner_T const* owner() const noexcept
    {
        // choosing const_cast away const and const_cast const back over repeteation of the same code
        return const_cast<Owner_T const*>( const_cast<member_of*>(this)->owner() );
    }
};

#undef NEEDS_BYTE_DEFINITION
}// namespace tosics::util
#endif // MEMBER_HPP 1
