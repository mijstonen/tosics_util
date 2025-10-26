#!/usr/local/bin/cpx
#|

    template <typename R, typename... T>
    std::tuple<T...>
function_args(R (*)(T...))
{
    return std::tuple<T...>{};
}
    template <typename R, typename... T>
    std::tuple<T...>
function_args(R (*)(T...),T... _t)
{
    return std::tuple<T...>{_t...};
}
    template <class C, typename R, typename... T>
    std::tuple<T...>
method_args(R (C::*)(T...))
{
    return std::tuple<T...>{};
}
    template <class C, typename R, typename... T>
    std::tuple<T...>
method_args(R (C::*)(T...),T... _t)
{
    return std::tuple<T...>{_t...};
}
    template <class C, typename R, typename... sigP>
    std::pair<std::pair<size_t,size_t>, std::tuple<sigP... > >
method_situhash_args(R (C::*)(sigP...))
{
    auto codes = SIGNATURETUPLE_HASH_CODES(sigP{}...);
    return {codes,std::tuple<sigP...>{}};
}
    template <class C, typename R, typename... sigP>
    std::pair<std::pair<size_t,size_t>, std::tuple<sigP... > >
method_situhash_args(R (C::*)(sigP...),sigP... _t)
{
    auto codes = SIGNATURETUPLE_HASH_CODES(sigP{_t}...);
    return {codes,std::tuple<sigP...>{_t...}};
}

double foo(double mi, int li, int div)
{
    return mi+double(li)/double(div);
}

// get the tuple type
// using FooArgType=decltype(function_args(foo));

// create a default-initialised tuple
// auto args =


#if 1
struct
AA
    {
        int m_divider;

        double foo(int a, int b)
        {
            return double(a+b)/double(m_divider);
        }

        int oof( double, double)
        {
            return 0;
        }

    };
#endif
#(
//INFO(VARVALS(function_args(foo),function_args(foo,0.0,0,0),function_args(foo,10.0,99,1000)));

    AA aa;
    aa.m_divider = 1'000'000;

    auto st = method_args(&AA::foo, 2, 137);
    INFO(VARVAL(st));
     INFO(VARVAL(SIGNATURETUPLE_HASH_CODES(int{2},int{137})));
    //auto hhst= method_situhash_args(&AA::foo);
    INFO(VARVALS(method_situhash_args(&AA::foo),method_situhash_args(&AA::foo,3,234) ));
    INFO(VARVALS(method_situhash_args(&AA::oof),method_situhash_args(&AA::oof,0.3,23.4) ));
#)
