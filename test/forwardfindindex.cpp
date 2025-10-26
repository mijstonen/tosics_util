#!/usr/bin/env cpx
#|
#(
//auto names= make_array_from<char const*,4>({"Michel","Josef","Johannes","Silvester"});
char const* tofind="Josef";
char const* nms[]={"Michel","Josef","Johannes","Silvester",tofind};
int index=ITEMS_IN(nms);
ForwardFindIndex( &index , nms);
INFO(VARVAL(index));
#)
