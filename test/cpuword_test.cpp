#!/usr/local/bin/cpx

    template <int _BITS> void
showCpuWorsTypeNames()
{
    // show the type names as being extracted from rtti

    typename cpuword<_BITS>::signed_type aSignedCpuWord;
    typename cpuword<_BITS>::unsigned_type aUnsignedCpuWord;
    INFO(VARVALS(_BITS,type_name(aSignedCpuWord),type_name(aUnsignedCpuWord)));
}

#!
    WITH_SCOPE_DECORATOR_LINES;

    //INFO(sizeof(void*)*CHAR_BIT,type_name(v));
    showCpuWorsTypeNames<8>();
    showCpuWorsTypeNames<16>();
    showCpuWorsTypeNames<32>();
    showCpuWorsTypeNames<64>();

    INFO(VARVAL(cpuword<64>::value));


    typename actual_cpuword::signed_type aSignedCpuWord;
    typename actual_cpuword::unsigned_type aUnsignedCpuWord;
    INFO("actual_cpuword",VARVALS(type_name(aSignedCpuWord),type_name(aUnsignedCpuWord)));

