#pragma once
//preserve.hpp
#ifndef PRESERVE_HPP_
#define PRESERVE_HPP_ 1

#include "util.hpp"
#include "info.hpp"


//# include <functional>


namespace tosics::util {

//IMPORTAND: this forward declaration is neccesary to make it work
/// @sa tosics::util::preserve< T, UWV...>
template <typename... T>   class preserve;
template <> class preserve<>;

/**
 @brief Base serves as single place to implement functionallity for the (variadic) preserve<...> template.
*/


class preserve_base
{
// configure OnBeforeRestore, disable when not needed because std::function causes a lot of overhead.
#define PRESERVE_HAS_ONBEFORERESTORE 0

  private:
    //bool m_restore=true;
    // a nameless struct of bitfields has the advantage that the datatype is extendable with
    //  control bits witout (to a certain amount) changing size of the datatype.
    struct {
        unsigned m_restore:1;
    };

#if PRESERVE_HAS_ONBEFORERESTORE
  private:
    static std::function<void(preserve_base*)> OnBeforeRestore_dummy_lambda()
    {
        return [&](preserve_base*)
        {
          //  DBG_INFO("dummy_lambda"); // for test and debug
        };
    };
    std::function<void(preserve_base*)> m_onBeforeRestore = OnBeforeRestore_dummy_lambda();

  protected:
    void runOnce_onBeforeRestore()
    {
        m_onBeforeRestore(this);
        m_onBeforeRestore = OnBeforeRestore_dummy_lambda();
    }
  public:
    //property (in/set)
    void onBeforeRestore(decltype(m_onBeforeRestore) const& _onBeforeRestore)
    {   m_onBeforeRestore= _onBeforeRestore;
    }
#else
    protected: void runOnce_onBeforeRestore(){

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
        _origin.restore(false);  // only the copied preserve instance will restore, not its origin
    }

    preserve_base(preserve_base&& _origin)
    : m_restore(true)
    {
        _origin.restore(false);  // only the copied preserve instance will restore, not its origin
    }

    ~preserve_base()
    {
        preserve_base::runOnce_onBeforeRestore();
    }


    //property (out/get)
    bool restore() const
    {
        return m_restore;
    }

    //property (in/set)
    void restore(bool const _restore)
    {
        m_restore= _restore;
    }

    void commit()
    {
        restore(false);
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

    bool changed() const
    {
      if ( r==v ) {
          // here the observed r is still the same as the stored v,
          // but maybe in the rest of the pack something changed.
          return preserve<UVW...>::changed();
      }
      // here: v!=r  aka, the observed value in r is changed against v
      return true;
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

    bool changed() const
    {
        return false;
    }
};


template <typename... T>  /*class*/ preserve<T...> make_preserve(T&... /* refs*/)
{
  ASSERT(false); // DO NOT CALL THIS METHOD, it is only supplied for the type system
  // needed in decltype of macro
}


} // namespace util
#define preserve_INSTANCE(...) decltype(tosics::util::make_preserve(__VA_ARGS__))

// Macro's for users

/// @brief Make a named preserve instance and list the objects to be preserved after it, see preserve demos 0 in util_demos.cpp
#define PRESERVE_IN(localPreserveInstanceName,...) preserve_INSTANCE(__VA_ARGS__) localPreserveInstanceName(__VA_ARGS__)

/// @brief Retrieve the standard name of the preserve instance, see preserve demos 1 and 2 in util_demos.cpp
#define LOCAL_MODIFIED_OBJECTS __anonymous__PreserveInstance__
//#define LOCAL_MODIFIED_OBJECTS AUTO_ID

/// @brief Users (by default) should use this macro, see preserve demos 1 and 2 in util_demos.cpp
/// @remark Should only used once in a compund statement, alternatively use PRESERVE_IN
#define LOCAL_MODIFIED(...) PRESERVE_IN(LOCAL_MODIFIED_OBJECTS,__VA_ARGS__)

#endif // PRESERVE_HPP_
