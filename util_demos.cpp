#include "utils.hpp"


void findIndex_demo()
{
    INFO("FastFindIndex_demo");

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
    findIndex_demo();
    return 0;
}
