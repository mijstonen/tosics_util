#!/usr/bin/env cpx

         // This is proto typing and research, of cause it should be refactored into cpx_stdphp.hpp as class EnumGenerator
         // What has begun as just showing the program arguments, has now turned into prototyping
         // C++ enum automation
         // -------------------
         // One big anoyance in C++ is the representation of enums, unless you have some fancy reflection tools (aka QT moc)
         // you cannot present enums out of the box. Here is some PHP based code generation that solves this.

 #define PHPVAR_ENUM_WORDS ${$enumClass."_words"}
 #define MAKE_ENUM(enumClassName,eNumNameChain) <? $enumClass=#enumClassName; PHPVAR_ENUM_WORDS=explode("|",#eNumNameChain); ?>
 // The only place where the enum is defined, '|' is used as separator because the C/C++ prepocessor cannot cope with ',' which it uses for argument separation.
 MAKE_ENUM(Dodo,a|b|c|d|e|g|h|i|stop)

 #define ENUM_CLASS   <?=$enumClass?>
 #define ENUM_STRINGS <?=$enumClass."_strings"?>
 #define ENUM_RANGE   <?=$enumClass."_range"?>
 #define ENUM_MAP     <?=$enumClass."_map"?>

 #define  $$$STRINGENUM_PAIR(i){ENUM_STRINGS[static_cast<int>(i)],i}
 #define STRINGENUM_PAIR(enumItemName) $$$STRINGENUM_PAIR(ENUM_CLASS::enumItemName)

 #define ENUM_FOREACH <?foreach(PHPVAR_ENUM_WORDS as $w){?>
 #define ENUM_ITEM <?=$w?>
 #define ENUM_ITEM_STRING <?='"'.$w.'"'?>
 #define ENUM_END_FOREACH <?}?><?PHP_MARKSRCLINE?>

 // define the enum
 enum class ENUM_CLASS:int{ null
    ENUM_FOREACH,ENUM_ITEM
    ENUM_END_FOREACH
 } nums;

 // make a range to iterate (in C++) over the item with a range based for loop
 ENUM_CLASS ENUM_RANGE[] = {ENUM_CLASS::null
    ENUM_FOREACH,ENUM_CLASS::ENUM_ITEM
    ENUM_END_FOREACH
 };

 // make a array containing the enum items in as C string litrals
 const char* ENUM_STRINGS[]{"null"
    ENUM_FOREACH,ENUM_ITEM_STRING
    ENUM_END_FOREACH
 };

 // map strings to the enum items
 map<string,decltype(nums)>ENUM_MAP{STRINGENUM_PAIR(null)
     ENUM_FOREACH,STRINGENUM_PAIR(ENUM_ITEM)
     ENUM_END_FOREACH
 };

    template
    <
        typename OS_T
    >
    OS_T&
 operator<< (OS_T& os_, const <?=$enumClass?> _eci)
 {
     return os_ << <?=$enumClass?>_strings[ static_cast<int>(_eci)];
 }

    template
    <
        typename IS_T
    >
    IS_T&
 operator>> (IS_T& is_, <?=$enumClass?> &eci_)
 {
    string eci_word;
    is_ >> eci_word;

    auto found_at=ENUM_MAP.find( eci_word);
    bool not_found( found_at== ENUM_MAP.end() );
    auto empty( <?=$enumClass?>::null );

    eci_= not_found? empty: found_at->second;
    return is_;
 }

 // create range   < ? =$enumClass>  < ?"${enumClass}

 <?
 function index_for( $index_var_name, $max_index_initializer,$srcFileAtCall,$srcLineAtCall) {
 echo<<<EOSSS
     const auto max_$index_var_name($max_index_initializer);
     for ( std::remove_const< decltype(max_$index_var_name)>::type $index_var_name(0);
         $index_var_name< max_$index_var_name;
         ++$index_var_name ) /* for body ( aka {....} ) must follow when used */
 EOSSS;
 PHP_ATMARKSL($srcFileAtCall,$srcLineAtCall); // fool the compiler as if where still on the first line of macro expansion
 }
 ?>
 #define INDEX_FOR(itr_var, max_expr) <?index_for(#itr_var,#max_expr,__FILE__,__LINE__);?>

#(
    INFO(VARVALS(ProgramArguments));
    {INDEX_FOR(a,ProgramArguments.size()){
            INFO(VARVALS(a,ProgramArguments[a]));
    }}
    double relativity[10][10];
    const unsigned range[]={0,1,2,3,4,5,6,7,8,9};

    INFO(VARVALS(relativity));
    for(auto i:range){
        for(auto j:range){
            relativity[i][j]=i*0.10+j*0.01;
        }
        INFO(VARVALS(i,relativity[i]));
    }

    vector<unsigned> primes{1,3,5,7,11,17,19,23,29};
    INFO("vector<unsigned>",VARVAL(primes));
    string s="print as one string";
    INFO(VARVAL(s));
    INFO(VARVALS(nums,Dodo_strings,Dodo_map));
    //round trip check
    for(auto ei:Dodo_range) {
        auto index= static_cast<int>(ei);
        auto key= Dodo_strings[index];
        auto value= Dodo_map[key];
        ASSERT(value==ei);
        INFO(VARVALS(ei,index,key));
    }
    INFO("\nReading user input and turn it into Dodo's, press 'stop' to stop");
    Dodo item;
    while ( cin>>item ) {
        INFO(VARVALS(item,static_cast<int>(item)));
        if( item==Dodo::stop ) {
            break;
        }
    }
#)
