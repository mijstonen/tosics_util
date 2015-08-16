#include "utils.hpp"
#include "info.hpp"
#include "preserve.hpp"
#include "error.hpp"
#include "statereport.hpp"


void preserve_demo_0()
{
  INFO("\npreserve_demo 0 for debugging");
  int i=5, j=20,k=21,l=22;
  { PRESERVE_IN(instance, i,j,k,l);  // basic declaring macro with explist name of util::preserve<...> object (aka instance)
    i=9;
    std::cout<<i<<std::endl;
    //instance.restore(false);
#if PRESERVE_ADVANCED
    instance.
        onBeforeRestore([&](util::preserve_base* _context_) {
            INFO("my lambda 0",VARVAL(i),VARVAL(j),VARVAL(k),VARVAL(l),VARVAL(_context_->restore()));
        });
#endif
  }
  std::cout<<i<<std::endl;
}

void preserve_demo_1()
{
    INFO("\npreserve_demo (always restore locally modified objects)");

    int a=1; int b=22222;
    float d = 123.456;
    int *pa= &a;
    std::string name("Michel");
    INFO("intial:",VARVAL(a),VARVAL(d),PTRVAL(pa),VARVAL(name));


    try {
        LOCAL_MODIFIED(a,d,pa,name);
#if PRESERVE_ADVANCED
        // Do something before restoring in case the body is left by some reason
        LOCAL_MODIFIED_OBJECTS.
            onBeforeRestore([&](util::preserve_base* _context_)
                {
                    INFO("my lambda 0",_context_->restore());
                    INFO("before restoring:",VARVAL(a),VARVAL(d),PTRVAL(pa),VARVAL(name));
                });
#endif
      INFO(VARVAL(LOCAL_MODIFIED_OBJECTS.restore()));
      // Modify
      d=456;  a=2;  pa= &b;  name="Peter";
      INFO("modified:",VARVAL(a),VARVAL(d),PTRVAL(pa),VARVAL(name));
      throw "oops";
    }
    catch ( const char* msg )
    {
      INFO(VARVAL(msg));
    }

    INFO("restored",VARVAL(a),VARVAL(d),PTRVAL(pa),VARVAL(name));
}

void preserve_demo_2(bool _fail)
{
    INFO("\npreserve_demo 2 (pseudo commit)");

    int a=1; int b=22222;
    float d = 123.456;
    int *pa= &a;
    std::string name("Michel");


    try {
        LOCAL_MODIFIED(a,d,pa,name);
        INFO("intial:",VARVAL(a),VARVAL(d),PTRVAL(pa),VARVAL(name));
        d=456;  a=2;  pa= &b;  name="Peter";
        INFO("modified:",VARVAL(a),VARVAL(d),PTRVAL(pa),VARVAL(name));

        if ( _fail ) {
            util::Throw_Error_Break("demontraing fail");
        }

        // no exceptions thrown, pseudo commit, by disabling the restore mechanism
        LOCAL_MODIFIED_OBJECTS.restore(false);
    }
    catch ( const char* excption_message )
    {
        INFO(VARVAL(excption_message),VARVAL(a),VARVAL(d),PTRVAL(pa),VARVAL(name), "stack unrolled");
    }

    // LOCAL_MODIFIED_OBJECTS go out of scope
    // unless restore is set to false, the
    // LOCAL_MODIFIED_OBJECTS destrucor
    // cases restoring the objects
    INFO((_fail?"restored:":"committed:"),VARVAL(a),VARVAL(d),PTRVAL(pa),VARVAL(name));
}

/*
preserve_demo_3  I will not write, because it is too complex.
But it is the use case for which the 'preserve' mechanism is intended.

Suppose you have a complex data structure and every method of it depends on a certain
member but is is not passed as argument (to limit number of arguments). Instead
the member is used as a inter-method communication mechanism. This works as long you
have no multiple (stackleves) of opertaion. Usuallyu however,those multiple stacklevels
should be assumed to keep the code safe.

example (in pseudo code)
{
  database db

  func child_data(&chdata,key)
  {
    LOCAL_MODIFIED(db.pointer)
    db.find(key);                        // moves detabase pointer to other location
    db.get_content(&chdata)
  }                                      // db.pointer is resoted


  db.find(location data)   // db pointer is now in location data
  db.get_content(&keys)
  for key:keys {
    child_data(&chdata,key)
    // do something with child data from
    // the (database) perspective of
  }

  This type of behaviour

*/


void findIndex_demo()
{
    INFO("\nfindIndex__demo");

    // const char* newSymbol="Case"; // will not find:
    const char* newSymbol="case"; // wil be found
    // const char* newSymbol="unchecked"; // wil be found, but is not checked


    static const char* newSymbol_and_nonParenKeywords[]=
          {  newSymbol /* is one of */
          ,     "try"
          ,     "do"
          ,     "switch"
          ,     "case"
          ,     "else"
          ,     "unchecked"
          };

    int array_size= ITEMS_IN(newSymbol_and_nonParenKeywords);
    decltype(array_size) found_index;


    INFO("Start search at",VARVAL(array_size-1));

    found_index= array_size;


    // search
    util::findIndex( &found_index, newSymbol_and_nonParenKeywords);


    INFO(VARVAL(found_index));

    // check if value is in set
    INFO(VARVAL(newSymbol), " in nonParenKeywords set ?", found_index? "Yes": "No");

    // easily reuse result, decide what to do and take advantage of the flexibility of switch

    switch ( found_index ) {
        case  5: INFO("else");
            break;

        case  4:
        case  3:
            INFO("case or switch");
            break;

        case  2: INFO("do");
            break;

        case  1: INFO("try");
            break;

        default:
          // The found index always is in the range of the passed array (aka newSymbol_and_nonParenKeywords)
          ASSERT(found_index<array_size);
          // It never drops below zero.
          ASSERT(found_index>=0);

          // Note that the ASSERT here is to document, findIndex() will check its output and meet de conditions above.

          if (found_index) {
              INFO("Warning, not all possible items checked");
          }
          else {
            // If zero than the intended search has not found the item.
            INFO(VARVAL(newSymbol),"is not found");
          }
    }
}

void info_demo()
{
    INFO("info demo");
  #if 1
    // foo example
    std::string aName="Hello";
    int count=14;
    unsigned long ulonghex=0xabcdef000000;
    int *pcount= &count;
    char control='Y';
    char highCtrl= -40;        // test signed char conversion  ( 256 - 40 => 216 )
    float Pi=3.14159265359;
    bool b=false;


    // Normally variables are used in the program
    // but when executing this program in release mode
    // the it would report 'unused variable ...'for each above.
    // That is suppressed below by void expressions that use the variables.
#   ifdef NDEBUG
    FAKE_USE(aName);    FAKE_USE(count);    FAKE_USE(ulonghex);    FAKE_USE(pcount);
    FAKE_USE(control);  FAKE_USE(Pi);       FAKE_USE(highCtrl);    FAKE_USE(b);
#   endif


    INFO(VARVAL(highCtrl),VARCHRNUM(highCtrl),VARCHRNUMHEX(highCtrl),VARVAL(b),123,0.123,!b);

    std::string alternative="Or write a ostream& operator << (ostream&,<your type>) operator for it.";

    // no brainer on variadic arguments
    INFO("Can put in","any value of any type",ENDL
        ,"that is also accepted",ENDL
        ,"by iostream.", alternative);


    // but INFO puts a newline after every call (like echo)
    INFO("INFO uses newer C++ techniques");
    INFO("like variadic template function arguments");
    INFO("and variadic macro argument lists");
    INFO("To make its own syntax, almost");
    INFO("trivial.");

#ifndef NDEBUG
    util::info( std::endl< char, std::char_traits<char> > );

    util::info( "Call info directly", "with util::info()", std::endl< char, std::char_traits<char> >
                , aName, count, pcount, control);
#endif
    INFO("Macro INFO is basically the the same but fades away in release mode ( defined(NDEBUG) )", ENDL );

    INFO("With macros: Better format and consistent display of variable names.", ENDL,
         VARVAL(aName),VARVAL(count),VARVAL(control));
    INFO("PTR... macros print the address value of the pointer and the value on that address", ENDL,
         PTRVALHEX(pcount),PTRVAL(pcount));
    pcount= nullptr;
    INFO("INFO handles NULL pointers (nullptr) well",PTRVAL(pcount));


    INFO_TO(std::cerr);

    INFO(VARVALHEX(ulonghex));
    INFO("As number:  ", VARCHRNUM(control),
         "    And same again as hex number:  ", VARCHRNUMHEX(control));
    INFO("INFO produces raw output for intended debugging purposes ONLY without formatting.");
    INFO("Like with:  ",VARVAL(Pi));
    INFO("Its a drop-in replacer for 'printf' (or ostream cout, cerr)",ENDL,
          "like debugging with the advantage", ENDL,
          "of consistant variable name and variable value display");
    INFO("It might improve the developers productivity");
    INFO("Besides that, it is a risc free check on that C++11 compilation is enabled.");


    char const* strContainsNewline{"Show here\nWhat happens if ""\\n"" is used directly"};

    INFO(VARVAL(strContainsNewline));

    //
    // The target stream does not need to print to console or file. It can be something
    // completely different. The example below uses a string stream together with INFO
    // to create a query.
    //

    constexpr int key=12;
    constexpr long double factor=100.0/7.0;

    std::ostringstream Query1;
    // write to Query
    {
    # if 0
      // use (old style) macro encapsulation, certain extended settings are not supported
      LOCAL_MODIFIED(INFO_STREAM_PTR);
      INFO_TO(Query1);
    # endif

    # if 0
      // control all settings locally, usefull if there are mulitple collections of settings to be managed, future save when more settings are added.
      LOCAL_MODIFIED(util::AppliedInfoSettingsPtr);
      util::InfoSettings mySettings;
      mySettings.ostreamPtr= &Query1;
      mySettings.customQuote="\"";
      util::AppliedInfoSettingsPtr= &mySettings;
    # endif

    # if 1
      // Control settings that are changed, future save when more settings are added.
      LOCAL_MODIFIED( util::AppliedInfoSettingsPtr->ostreamPtr
                    , util::AppliedInfoSettingsPtr->customQuote
                    );

      # if 1
      util::AppliedInfoSettingsPtr->ostreamPtr= &Query1;
      # else
      // deliberately corrupting to test/show STATEREPORT and validateFailed()
      util::AppliedInfoSettingsPtr->ostreamPtr= nullptr;
      # endif
      # if 1
      util::AppliedInfoSettingsPtr->customQuote="\"";
      # else
      // deliberately corrupting to test/show STATEREPORT and validateFailed()
      util::AppliedInfoSettingsPtr->customQuote="";
      # endif
    # endif

      // no need for handling the validateFailed() outcome, that is done by the STATEREPORT macro.
      if ( !STATEREPORT(util::AppliedInfoSettingsPtr->validateFailed()) ) {

          INFO("select    *"
              ,"  from    tX, tY"
              ," where    tX.key=", key
              ,"   and    tY.fact< ", factor
              ," order by tX.at, tY.lo"
              );
      }
    }

    // show query string that was created
    INFO(VARVAL(Query1.str()));

    std::ostringstream Query2;
    // write to Query
    {
      INFO2STREAM( Query2
          ,"-- second, now acros multiple lines",ENDLINES(4)




          ,"select    *",ENDL
          ,"  from    tX, tY",ENDL
          ," where    tX.key=", key,ENDL
          ,"   and    tY.fact< ", factor,ENDL
          ," order by tX.at, tY.lo"
          );
    }

    // show query string that was created
    INFO(VARVAL(Query2.str()));

#endif

    float interest= 3.42;
    long double Pii= 9978066541.0 / 3176117225;
    float spi= 62103.0/19768.0;
    double eenderde1= static_cast<double>(1.0/3.0);
    float eenderde2= static_cast<float>(1.0/3.0);

    INFO("direct floats",eenderde1,eenderde2);

    CERROR("write to stderr",VARVAL(interest),ENDL,VARVAL(Pii),VARVAL(spi),ENDL,VARVAL(eenderde1),VARVAL(eenderde2));

    INFO("Back on stdout");
}

void STATEREPORT_demo()
{
  {// extra local body
    #if 1
    LOCAL_MODIFIED(util::stateReport_StreamPtr);
    std::ofstream myStateReports("statereport.txt");
    ASSERT(myStateReports);                               // created stream must be valid to continue
    util::stateReport_StreamPtr= &myStateReports;
    // OR
    //auto originalStreamPtr= util::stateReport_exchange_StreamPtr(&myStateReports);
    #endif


  //{ notice that 0 is ignored
    INFO(ENDL,"silent");
    int silent='q'; /* 'q' or 'Q' or 0 */
    for ( auto function_return_value : {-3,-2,-1,0,1,2,3} ) {
        INFO(VARVAL(STATEREPORT(function_return_value,silent)));
    }

    INFO(ENDL,"notify");
    int notify='N'; /* 'n' or 'N' or 1 */
    for ( auto function_return_value : {-3,-2,-1,0,1,2,3} ) {
        INFO(VARVAL(STATEREPORT(function_return_value,notify)));
    }

    INFO(ENDL,"error");
    int error='2'; /* 'e' or 'E' or 2 */
    for ( auto function_return_value : {-3,-2,-1,0,1,2,3} ) {
        INFO(VARVAL(STATEREPORT(function_return_value,error)));
    }

    INFO(ENDL,"all, except 0");
    int all='A'; /* 'a' or 'A' or 3 */
    for ( auto function_return_value : {-3,-2,-1,0,1,2,3} ) {
        INFO(VARVAL(STATEREPORT(function_return_value,all)));
    }
  //} notice that 0 is ignored
  //{ to not ignore 0, pass a empty exclusion list
    INFO(ENDL,"all, including 0");
    for ( auto function_return_value : {-3,-2,-1,0,1,2,3} ) {
        INFO(VARVAL(STATEREPORT(function_return_value,SR_EXCLUDE_ALL(),'n')));
    }
  //} to not ignore 0
  //{ selective reporting
    INFO(ENDL,"typical use");
    for ( auto function_return_value : {-3,-2,-1,0,1,2,3} ) {
        auto STATEREPORT_function_return_value= STATEREPORT(function_return_value,SR_EXCLUDE_0_AND(-2,1,3));  // _what is not specified, it defaults to 3 (same as 'a' or 'A')
        INFO(VARVAL(STATEREPORT_function_return_value));
        switch ( STATEREPORT_function_return_value ) {
            case -2:
                INFO("case -2 handled");
                break;

            case 1:
                INFO("case 1 handled");
                break;

            case 3:
                INFO("case 3 handled");
                break;

            default:
                INFO("Same ",VARVAL(STATEREPORT_function_return_value)," take proper action");
        }
    }
  //} selective reporting
     //util::stateReport_exchange_StreamPtr(originalStreamPtr);
  }// extra local body

    // And now the stateReport_streamPtr is back
    auto theEnd=[&]()
    {
      return -2;
      //return 0;
    };
    // if no STATEREPORT is issued when calling (lambda) function theEnd()
    if ( !STATEREPORT(theEnd()) ) {
      INFO("theEnd() returned 0, meaning: it succeed.");
    }
}


int main(int , char **)
{
    INFO("Util demos");
    preserve_demo_0();
    preserve_demo_1();
    preserve_demo_2(/* _fail = */false );
    preserve_demo_2(/* _fail = */true );
  #if 1
    findIndex_demo();
    info_demo();
    STATEREPORT_demo();

    LOCAL_MODIFIED();
    INFO("Do somesthing");
  #endif

    return 0;
}
