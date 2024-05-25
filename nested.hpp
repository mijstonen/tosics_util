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

//:nested_of    base class for a inner object that stores the offset to its owning object and has outer().
//              Simplifies interaction between nested classes.
//              If the owning (and enclosing) class is copied or moved to an other memory location,
//              the relation remains in tact. Either if a inner is copied to another inner, the outer<-inner
//              relation remains in tact.
//
//  The stored offset (in nested_of<?>::Offset_T) is likely need less then sizeof(int64) bytes.
//



namespace tosics::util {

    using byte_t = std::byte;

        template<class Inner_T,class Outer_T>
        ptrdiff_t
    offset__from_inner_to_outer(Inner_T const* _pI, Outer_T const* _pO)
    {
        auto pO= static_cast<byte_t const*>(static_cast<void const*>(_pO));
        auto pI= static_cast<byte_t const*>(static_cast<void const*>(_pI));

        ptrdiff_t return_value{ pI- pO };

        return return_value;
    }
        template<class Outer_T,class Inner_T>
        Outer_T const*
    pointer__from_inner_to_outer(Inner_T const* _pI, ptrdiff_t _offset)
    {
        byte_t const* pOuterByteAdr= static_cast<byte_t const*>(static_cast<void const*>( _pI))-_offset;
        return  static_cast<Outer_T const*>( static_cast<void const*>( pOuterByteAdr));
    }

    template<
        typename Outer_T,
        typename Offset_T = int16_t,
        ptrdiff_t BEGIN_OFFSET = std::numeric_limits<Offset_T>::min(),
        ptrdiff_t END_OFFSET   = std::numeric_limits<Offset_T>::max()
    >
    class
nested_of
{
  protected:
        Offset_T
    offset_to_outer(Outer_T const* _outer) noexcept
    {
        ptrdiff_t  return_value{offset__from_inner_to_outer( this, _outer)};

        // If these ASSERTs fail, consider use of a Offset_T with a largher range (aka int32_t i.s.o. default unt16_t)
        ASSERT( return_value>= BEGIN_OFFSET );
        ASSERT( return_value< END_OFFSET );
        return static_cast<Offset_T>(return_value);
    }

        Offset_T
    _M_offset_to_outer = END_OFFSET;

    nested_of() = delete;

    nested_of( Outer_T* _outer) noexcept
    : _M_offset_to_outer(offset_to_outer(_outer))
    {
        // read comments above
    }

    // if _M_offset_to_outer is already initialized, it should not be changed by copying, that's incorrect
    nested_of(const nested_of& _from) noexcept
    : _M_offset_to_outer( _M_offset_to_outer)
    {
        ASSERT( _M_offset_to_outer!=_from._M_offset_to_outer ); // only subclass data should be copied
    }

    nested_of& operator = (const nested_of& _from) noexcept
    {
        // emplace copy, so = behaves like copy constructor
        new(this) nested_of(_from);
        return *this;
    }

    // NOTE: Members are always lvalues, it (probably) does not make sense to move (from) them.
    nested_of(nested_of&& _from) = delete;

        void
    outer(Outer_T const* _outer) noexcept
    {
        _M_offset_to_outer= offset_to_outer(_outer);
    }

        Outer_T const*
    outer() const noexcept
    {
        static_assert( std::is_signed<Offset_T>::value ); // require positive and negative offset handling
        static_assert( sizeof(Offset_T)<=sizeof(ptrdiff_t) ); // Offset_T must fint into ptrdiff_t
        return pointer__from_inner_to_outer<Outer_T>( this, _M_offset_to_outer );
    }

        Outer_T *
    mutable_outer() const noexcept     // NOTE: allows mutation of outer object from const inner object.
    {
        return const_cast<Outer_T*>(const_cast<nested_of const*>(this)->outer());
    }

};

#if 1
        template<typename Outer_T> using
    nested_of8_t = nested_of<Outer_T,int8_t>     // small offset 2^8 -1
    ;
        template<typename Outer_T> using         // short offset 2^16 -1
    nested_of16_t= nested_of<Outer_T,int16_t>
    ;
        template<typename Outer_T> using
    nested_of32_t= nested_of<Outer_T,int32_t>    // normal offset  2^32 -1
    ;
        template<typename Outer_T> using
    nested_of64_t= nested_of<Outer_T,int64_t>    // huge offset 2^64 -1
;
#endif

#undef NEEDS_BYTE_DEFINITION
}// namespace tosics::util
#endif // MEMBER_HPP 1
