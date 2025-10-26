#pragma once
//type_traits.hpp
#ifndef TYPE_TRAITS_
#  define TYPE_TRAITS_ 1

// See also: showtraits.cpp test

namespace tosics::util
{

//:is_iterable
// Meta type predicate that result to true when T has methods begin() and end() otherwise false.
// We need this as a minimum requirement for CONTAINER_T, otherwise compilation is as intended to fail.


    template
    <   typename
    ,   typename = std::void_t<>
    >
struct is_iterable
        : std::false_type {}
;

    template
    <   typename T
    >
struct is_iterable
    <   T
    ,   std::void_t
        <   decltype(std::declval<T&>().cbegin())
        ,   decltype(std::declval<T&>().cend())
        >
    >   : std::true_type {}
;

template <typename T>
inline constexpr bool is_iterable_v = is_iterable<T>::value;


// Container suited for Append_splitted()
    template <typename CONTAINER_T, typename = void> struct
is_appendable_at_end : std::false_type {};
// but when
// having methods push_back() emplace_back() and back()
    template <typename CONTAINER_T/*, specialized*/> struct
is_appendable_at_end<CONTAINER_T,
    std::void_t<  decltype( std::declval<CONTAINER_T>().push_back(  std::declval<typename CONTAINER_T::value_type const&>()) ),
                  decltype( std::declval<CONTAINER_T>().emplace_back() ),
                  decltype( std::declval<CONTAINER_T>().back() )  >
> : std::true_type {};

#if 0
// but when
// having methods push_back() emplace_back() and back()
    template <typename CONTAINER_T/*, specialized*/> struct
is_appendable_at_end<CONTAINER_T,
    std::void_t<  decltype( std::declval<CONTAINER_T*>()->push_back() ),
                  decltype( std::declval<CONTAINER_T*>()->emplace_back() ),
                  decltype( std::declval<CONTAINER_T*>()->back() )  >
> : std::true_type {};
#endif

template<typename S,typename T,
    /*common*/typename = void>
struct is_to_stream_writable
/*unspecialized common*/
: std::false_type {};

#if 1
template<typename S,typename T
    /*specialized*/          >
struct is_to_stream_writable
<S,T,  std::void_t<decltype( std::declval<S&>() << std::declval<T>() )>  >
: std::true_type {};
#endif

#if 0
// FIXME: WIP
/*
For writing conainer types again (I still don't know why it worked before, but not now),
Containers/Ranges (condition:is_iterable_v) should sayd to be writeable if there elements
are writable.
So write a specialization of is_to_stream_writable for that scenario. This will
compiletime select the correct now_stream_object template function (info.hpp:350..400)
*/
    template<
        typename S,
        template<typename...T> typename Container_T,
        typename... T
//        ,typename std::enable_if_t<is_iterable_v<Container_T<T...> >, int> = 0
    >
struct is_to_stream_writable
: std::true_type {};


#endif

template<typename S,typename Ref_T,        /*  actual T& to formal Ref_T  */
    /*common*/typename = void    >
struct is_from_stream_readable
/*unspecialized common*/
: std::false_type {};

template<typename S,typename Ref_T
    /*specialized*/              >
struct is_from_stream_readable
<S,Ref_T, std::void_t<decltype( std::declval<S&>() >> std::declval<Ref_T>() )>  >
: std::true_type {};


template<typename S,typename T>
struct is_from_to_stream_readable_writable
: std::conjunction< is_to_stream_writable<S,T>, is_from_stream_readable<S,T&> > {};

#if 1
# define _INLINE_VAR_ inline
#else
# define _INLINE_VAR_
#endif

//template<typename S,typename T>
//_INLINE_VAR_ constexpr bool is_iterable_v= is_iterable<S,T>::value;

template<typename S,typename T>
_INLINE_VAR_ constexpr bool is_to_stream_writable_v= is_to_stream_writable<S,T>::value;

template<typename S,typename T>
_INLINE_VAR_ constexpr bool is_from_stream_readable_v= is_from_stream_readable<S,T>::value;

template<typename S,typename T>
_INLINE_VAR_ constexpr bool is_from_to_stream_readable_writable_v= is_from_to_stream_readable_writable<S,T>::value;


// Custom type trait to detect std::ostream or its derivatives
template<typename OS_T>
struct is_std_ostream : std::is_base_of<std::ostream, OS_T> {};

template<typename OS_T>
_INLINE_VAR_ constexpr bool is_std_ostream_v= is_std_ostream<OS_T>::value;

// Concept using the custom type trait
template<typename OS_T>
concept std_ostream = is_std_ostream<OS_T>::value;


} // namespace cpx::util

#endif // TYPE_TRAITS_
