#pragma once
//type_traits.hpp
#ifndef TYPE_TRAITS_
#  define TYPE_TRAITS_ 1



namespace tosics::util
{

//:is_iterable
// Meta type predicate that result to true when T has methods begin() and end() otherwise false.
// We need this as a minimum requirement for CONTAINER_T, otherwise compilation is as intended to fail.

// Container that can iterate over its items
    template <typename ITERABLE_T,typename = void> struct
is_iterable : std::false_type {};
// but when
// having begin() and end() that each should return the container iterator type.
    template <typename ITERABLE_T /*, specialized*/>struct
is_iterable<ITERABLE_T,
    std::void_t<    decltype( std::declval<ITERABLE_T>().begin() ),
                    decltype( std::declval<ITERABLE_T>().end() )     >
> : std::true_type {};

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
template<typename S,typename T
    /*specialized*/          >
struct is_to_stream_writable
<S,T,  std::void_t<decltype( operator<< (std::declval<S&>(), std::declval<T>()) )> >
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

template<typename S,typename T>
_INLINE_VAR_ constexpr bool is_iterable_v= is_iterable<S,T>::value;

template<typename S,typename T>
_INLINE_VAR_ constexpr bool is_to_stream_writable_v= is_to_stream_writable<S,T>::value;

template<typename S,typename T>
_INLINE_VAR_ constexpr bool is_from_stream_readable_v= is_from_stream_readable<S,T>::value;

template<typename S,typename T>
_INLINE_VAR_ constexpr bool is_from_to_stream_readable_writable_v= is_from_to_stream_readable_writable<S,T>::value;

} // namespace cpx::util

#endif // TYPE_TRAITS_
