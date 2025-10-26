#!/usr/local/bin/cpx

template <size_t N, typename T>
size_t items_in(T const (&)[N])
{
    return N;
}


    void
words(char const* _ws[])
{
//    INFO(VARVALS(array<const char*,7>(_ws,7)));
}


#if 0
    template <class T, std::size_t N>
    /*constexpr*/
operator std::array<std::remove_cv_t<T>, N> (T (&a)[N])
{
    return to_array(_t);
}
#endif

    template <size_t N, typename T>
    auto
mka(T const (&_t)[N])
{
    return to_array(_t);
}

#!
char const* nws[]= {"_if","else","for","while","until","break","return"};
auto three={1.0,2.1,3.2};
auto ws=to_array(nws);
//words(ws);
INFO(VARVAL(mka(nws)));
INFO(VARVAL(Items_in("123456789")));
INFO(VARVAL(Items_in(three)));
INFO(VARVALS(Items_in(nws)));


char const *member="break";
char const* kws[]= {member,"_if","else","for","while","until","break","return"};
int  NumberOfItemsIn_indexOut= Items_in( kws);

FindIndex( &NumberOfItemsIn_indexOut, kws );
if (NumberOfItemsIn_indexOut) {
    INFO("Found");
}
else {
    INFO("Not found");
}





//if ( as_member( a).in( abcde_set)
