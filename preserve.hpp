#ifndef PRESERVE_HPP_
#define PRESERVE_HPP_ 1

//#include "utils.hpp"
#include "info.hpp"

#define PRESERVE_ADVANCED 1


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

#if PRESERVE_ADVANCED
    std::function<void(preserve_base*)> m_onBeforeRestore = [&](preserve_base*){}; // dummy lambda
    bool m_should_call_onBeforeRestore=false;

  protected:
    void maybe_run_onBeforeRestore()
    {
        if ( !m_should_call_onBeforeRestore ) {
             return;
        }
        m_onBeforeRestore(this);
    }
#endif

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
    decltype(m_restore) restore() const
    {
        return m_restore;
    }

    /// @brief Set restore property. If set to false, the preserved objects will not be restored
    void restore(const decltype(m_restore)& _restore)
    {
        m_restore= _restore;
    }


#if PRESERVE_ADVANCED
    /// @brief set handler (lambda) that is fired just before start restoring values
    void onBeforeRestore(const decltype(m_onBeforeRestore)& _onBeforeRestore)
    {
        m_onBeforeRestore= _onBeforeRestore;
        should_call_onBeforeRestore(true);
    }


    #if 0
    decltype(m_onBeforeRestore) onBeforeRestore() const
    {
        return m_onBeforeRestore;
    }
    #endif


    /// @brief Set should_call_onBeforeRestore property
    void should_call_onBeforeRestore(const decltype(m_should_call_onBeforeRestore)& _should_call_onBeforeRestore)
    {
        m_should_call_onBeforeRestore= _should_call_onBeforeRestore;
    }

    /// @brief Get should_call_onBeforeRestore property
    decltype(m_should_call_onBeforeRestore) should_call_onBeforeRestore() const
    {
        return m_should_call_onBeforeRestore;
    }
#endif

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
  @remark  Be cautios when applying LOCAL_MODIED or PRESERVE_IN macros to global variables in combination with threads.
           It is safer to use thread_local variables and apply to them.
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
#if PRESERVE_ADVANCED
        preserve_base::maybe_run_onBeforeRestore();
#endif
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
/// @remark Should only used once in a compund statement, alternatively use PRESERVE_IN
#define LOCAL_MODIFIED(...) PRESERVE_IN(LOCAL_MODIFIED_OBJECTS,__VA_ARGS__)

#endif // PRESERVE_HPP_
