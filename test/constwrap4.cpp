#!/usr/local/bin/cpx
#|

struct ConstReturnOverloadTag{
}
;
    extern ConstReturnOverloadTag
WithConstReturn
;
    ConstReturnOverloadTag
WithConstReturn
;

    template<class C,typename R,typename L>
    R const&
const_wrap_callable( L l, C const& c)
{
    return const_cast<R const&>(l(const_cast<C&>(c)));
}

  PHP_BEGIN
    function php_const_wrap(string $id, string $declaration,string $expressions)
    {
    PHP_HERE_BEGIN(1)
      template<class C,typename R,typename... A> R const&
      const_wrap_${id}(C const& c, $declaration,A&&...a)
      {
              auto
          call_expressions=[&](C& cc)->R&
          {
              $expressions
          }
          ;
              return
          const_wrap_callable< C,R,decltype(call_expressions) >( call_expressions, c)
          ;
      } // const_wrap

PHP_HERE_END(1)
    } // php_const_wrap

    php_const_wrap( "function","R& (*f)(C&,A&&...)", <<< EOS1
#@
        return (*f)(cc,std::forward<A>(a)...);
EOS1 );
    php_const_wrap( "method" ,"R& (C::*f)(A&&...)", <<< EOS2
#@
        return (cc.*f)(std::forward<A>(a)...);
EOS2 );
  PHP_END


#if 0
    template<class C,typename R,typename... A> R const&
const_wrap(C const& c, R& (*f)(C&,A&&...),A&&...a)
{
        auto
    call_expression=[&](C& cc)->R& {return (*f)(cc,std::forward<A>(a)...);}
    ;
        return
    const_wrap_callable< C,R,decltype(call_expression) >( call_expression, c)
    ;
}
    template<class C,typename R,typename... A> R const&
const_wrap(C const& c, R& (C::*f)(A&&...),A&&...a)
{
        auto
    call_expression=[&](C& cc)->R& {return (cc.*f)(std::forward<A>(a)...);}
    ;
        return
    const_wrap_callable< C,R,decltype(call_expression) >( call_expression, c)
    ;
}
#endif
/*
 To guarantee the origin can not be modified by modifying functions after wrapping const cast,
 duplicate can be used to duplicate the origin to the copy location, where the application
 can manage the storage of that copy. After the (potentionally) modifying function is called,
 the object on the copy location might be effected instead of the origin. When and if to use
 it is up to the application.
 Needless to say that this allows to use modifying methods of a class to get results from (otherwhise)
 modified origin. This is usefull for implementing dry run's (assuming polite programming and methods
 do not affect data outside the class instance).
 */

template<class C>
C const& duplicate(C* pcopy_, C const& _origin)
{
    ASSERT(pcopy_);
    pcopy_-> ~C();
    *pcopy_= _origin;
    return *pcopy_;
}

struct X
{
    X *m_pcopy{nullptr};

        int
    u=1
    ;
        int&
    get()
    {
        return u;
    }

    //using method_type = int& (X::*)();

#if 1
        int const&
    get() const
    {
        return const_wrap_method( duplicate(m_pcopy, *this), &X::get);
    }
#endif
};

int& f(int& g)
{
    return g;
}

#(
        X
    x
    ;
        X
    workera
    ;
    x.m_pcopy = &workera;

        int&
    v=x.get()
    ;
        X const&
    crx=x
    ;
        int const&
    w=crx.get()
    ;
    INFO(VARVALS(v,w));
#if 1
    int h=5;
    int const& crh=h;
    int const& cri= const_wrap_function( crh, &f);
    INFO(VARVAL(cri));
#endif
#)
