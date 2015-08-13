#include "preserve.hpp"   // other projects should refer to it as "util/preserve.hpp"

void preserve_demo_0()
{
  INFO("\npreserve_demo 0 for debugging");
  int i=5;
  { PRESERVE_IN(instance, i);  // basic declaring macro with explist name of util::preserve<...> object (aka instance)
    i=9;
    std::cout<<i<<std::endl;
    instance.restore(false);
  }
  std::cout<<i<<std::endl;
}

void preserve_demo_1()
{
    INFO("\npreserve_demo (always resore locally modified objects)");

    int a=1; int b=22222;
    float d = 123.456;
    int *pa= &a;
    std::string name("Michel");
    INFO("intial:",VARVAL(a),VARVAL(d),PTRVAL(pa),VARVAL(name));

  { LOCAL_MODIFIED(a,d,pa,name);

    INFO(VARVAL(LOCAL_MODIFIED_OBJECTS.restore()));
    // Modify
    d=456;  a=2;  pa= &b;  name="Peter";
    INFO("modified:",VARVAL(a),VARVAL(d),PTRVAL(pa),VARVAL(name));
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


int main(int , char **)
{
    printf("Util demos\n");
    preserve_demo_0();
    preserve_demo_1();
    preserve_demo_2(/* _fail = */false );
    preserve_demo_2(/* _fail = */true );
    findIndex_demo();

    return 0;
}
