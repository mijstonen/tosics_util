#ifndef PRESERVE_HPP_
#define PRESERVE_HPP_ 1

#include "utils.hpp"

namespace util {

//IMPORTAND: this forward declaration is neccesary to make it work
/// @sa util::preserve< T, UWV...>
template <typename... T>   class preserve;
template <> class preserve<>;

/**
 @brief Base serves as single place to implement functionallity for the (variadic) preserve<...> template.
*/
class preserve_base
{
  private:
    bool m_restore=true;


  public:

    preserve_base()
    : m_restore(true)
    {
    }


    preserve_base(preserve_base& _origin)
    : m_restore(true)
    {
        _origin.restore(false);  // only the copied preserve instance will resote, not its origin
    }

    preserve_base(preserve_base&& _origin)
    : m_restore(true)
    {
        _origin.restore(false);  // only the copied preserve instance will resote, not its origin
    }


    /// @brief Get restore property.
    bool restore() const
    {
        return m_restore;
    }

    /// @brief Set restore property. If set to false, the preserved objects will not be restored
    void restore(const bool& _restore)
    {
        m_restore= _restore;
    }

};

/**
 @brief use RAII to "preserve" data and automatically restore it at the end of the scope
 @sa LOCAL_MODIFIED

 preserve offers a powerfull jet easy to use and safe programming interface to ensure that prior content of a object
 which content will change in the scope - where the preserve object is used - will be restored when the scope is left
 no matter how it is left ( return, exception e.g. ).

 To preserve a single object a template pointer is required to remember the address of the object and a template object
 to copy the original value. When leaving the scope the destructor of preserve restores the original value. By use of
 C++11 variadic template argument techniques, any amount of multiple objects can be "preserved".

 @remark  The variadic template preserve<...> is not intended to be used directly.
          Its use is encapsulated in macros.
            PRESERVE_IN
            LOCAL_MODIFIED_OBJECTS
            LOCAL_MODIFIED

  @remark  To use it '#include <util/preserve.cpp>'
 */
template<typename T, typename... UVW>
class preserve<T,UVW...> : public preserve<UVW...>
{
  protected:
    T v, &r;

  public:
    preserve(T& _v, UVW&... _uvw)
    : preserve<UVW...>(_uvw...), v(_v), r(_v)
    {}

    preserve(preserve&  _preserve_push)
    : preserve<UVW...>( _preserve_push )
    , v(_preserve_push.v), r(_preserve_push.r)
    {
    }

    preserve(preserve&&  _preserve_push)
    : preserve<UVW...>( std::move(_preserve_push) )
    , v(std::move(_preserve_push.v)), r(_preserve_push.r)
    {
    }

    ~preserve()
    {
        if ( preserve_base::restore() ) {
            r= v;
        }
        //compiler implicitely calls preserve_push<UVW...>::~preserve_push();
    }
};
/// @brief root of variadic template preserve<...>
template <>
class preserve<> : public preserve_base
{
public:
    preserve()
    : preserve_base()
    {
    }

    preserve(preserve&  _preserve_push)
    : preserve_base( _preserve_push)
    {
    }

    preserve(preserve&&  _preserve_push)
    : preserve_base( std::move(_preserve_push))
    {
    }
};


template <typename... T>  class preserve<T...> make_preserve(T&... refs)
{
  ASSERT(false); // DO NOT CALL THIS METHOD, it is only supplied for the type system
  // needed in decltype of macro
}


} // namespace util

#define preserve_INSTANCE(...) decltype(util::make_preserve(__VA_ARGS__))
// Macro's for users

/// @brief Make a named preserve instance and list the objects to be preserved after it, see preserve demos 0 in util_demos.cpp
#define PRESERVE_IN(localPreserveInstanceName,...) preserve_INSTANCE(__VA_ARGS__) localPreserveInstanceName(__VA_ARGS__)

/// @brief Retrieve the standard name of the preserve instance, see preserve demos 1 and 2 in util_demos.cpp
#define LOCAL_MODIFIED_OBJECTS __anonymous__PreserveInstance__

/// @brief Users (by default) should use this macro, see preserve demos 1 and 2 in util_demos.cpp
#define LOCAL_MODIFIED(...) PRESERVE_IN(LOCAL_MODIFIED_OBJECTS,__VA_ARGS__)


#endif // PRESERVE_HPP_
