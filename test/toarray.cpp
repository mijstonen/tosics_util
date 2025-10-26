#!/usr/local/bin/cpx
#|

/*
 B0208
 This test is to demonstrate that it is possible to create container holding types entirely at compiletime with fixed length arrays.
 But what is special that there is no need to separately specify the array size. It is infered from the initialization with the items.
 This is very usefull to define fixed size (altough flexible programmable) data structurs (aka signals/slot circuits).
 But this way they can hold their items in flat lineair arrays, that are constructed on stack or globally and need no heap memeory,
 altough could also use heap memory / being allocated on the heap.

 It so (when heap usgae is minimized) possible to create very fast applications.
*/

template <
        class _containerT
    >
    struct
holder
;


    template <
        class _containerT
    >
    struct
holder
{
        _containerT m_items;

        void show() {
            INFO(VARVALS(sizeof *this, m_items));
        }
};



    template<
        typename... _itemsP
    >
    struct
holder2
{
    // See example on https://en.cppreference.com/w/cpp/language/sizeof...
    std::array<  std::common_type_t<_itemsP...>, sizeof...(_itemsP) > m_items;
    char c,d;

       constexpr
    holder2(_itemsP... _items)
    : m_items{ std::to_array({_items...})}
    {
    }

    void show() {
        INFO(VARVALS(sizeof *this, m_items));
    }
};


struct AAA
{
    int i;
    double d;
    string s;
    float f;
};

#(
 WITH_SCOPE_DECORATOR_LINES;
 holder
    numbers{
      to_array(
        {1,2,3,4,5}
      )
    };
    numbers.show();

    holder2 numbers2{-2,-3,-6};
    numbers2.show();
    auto ofs=offsetof( decltype(numbers2),d);
    INFO(VARVALS(ofs,& decltype(numbers2)::d));

    int normal[]{1,5,7,1};
    INFO(VARVALS(sizeof normal,normal));
#)


// use of inheritance for containment is possible but cumbersome
#if 0
    template<
        typename... _itemsP
    >
    struct
holder3
: std::array< std::common_type_t<_itemsP...>, sizeof...(_itemsP) >
{
       constexpr
    holder3(_itemsP... _items)
    : std::array< std::common_type_t<_itemsP...>, sizeof...(_itemsP) >{std::forward<std::common_type_t<_itemsP...>>(_items)...}
    {
    }

    void show() {
        INFO(VARVALS(sizeof *this, *this));
    }
};
#endif
