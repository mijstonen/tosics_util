#!/usr/bin/env cpx


#define INVALS(...) INFO(VARVALS(__VA_ARGS__))

    union 
Endian 
{
  private:
    const uint64_t i;
    const uint8_t  cc[8]; // == {0xef,0xcd,0xab,0x89,0x67,0x45,0x23,0x1} for big enian

    uint8_t const* begin() const {return cc;} 
    uint8_t const* end()   const {return begin()+(sizeof cc);}

    bool me_equal_to(decltype(cc) const& dd) const
    {
        return ( memcmp(cc,dd,sizeof cc)==0 );
    }
    
  public:
    Endian()
    : i(0x7061524334251607)
    {
    }
       
    bool is_Big() const
    {
        decltype(cc) dd={0x70,0x61,0x52,0x43,0x34,0x25,0x16,0x07};
        return me_equal_to( dd);
    }

    bool is_Little() const
    {
        decltype(cc) dd={0x07,0x16,0x25,0x34,0x43,0x52,0x61,0x70};
        return me_equal_to( dd);
    }
    
    
    void play() const
    {
        INVALS(i, is_Big()!=is_Little());
        
        // show byte layout
        for(auto const& c: *this){
            INFO(PTRCHRNUMHEX(&c));
        }
        cout<<"\nWrite as initializers for dd forward and backward, which is which depends on endian.\n";
        
        // generate forward initialization
        cout<< '{';
        uint8_t const* p= begin();
        for(;;){
            cout<<"0x"<<setw(2)<<setfill('0')<<hex<< static_cast<unsigned>(*p);
            if ( ++p == end() ) {
                cout<<"};"<<endl;
                break;
            }
            //else
            cout<<",";
        }
        
        // generate back initialization
        cout<< '{';
        for(;;){
            --p;
            cout<<"0x"<<setw(2)<<setfill('0')<<hex<< static_cast<unsigned>(*p);
            if ( p == begin() ) {
                cout<<"};"<<endl;
                break;
            }
            //else
            cout<<",";
        }
        
    }
};

#if 0
    uint8_t* c= (uint8_t*)&i;
    
    INFO(PTRCHRNUMHEX(c));
    ++c;
    INFO(PTRVAL(c),VARCHRNUMHEX(*c));
    ++c;
    INFO(PTRVAL(c),VARCHRNUMHEX(*c));
    ++c;
    INFO(PTRVAL(c),VARCHRNUMHEX(*c));
    ++c;
    INFO(PTRVAL(c),VARCHRNUMHEX(*c));
    ++c;
    INFO(PTRVAL(c),VARCHRNUMHEX(*c));
    ++c;
    INFO(PTRVAL(c),VARCHRNUMHEX(*c));
    ++c;
    INFO(PTRVAL(c),VARCHRNUMHEX(*c));
    ++c;
    

    bool return_value= ( (*((uint8_t*)(&i)) )==0xef )? true: false;
    return return_value;
#endif
#!
    const Endian endian;
    endian.play();
    INVALS( endian.is_Big(), endian.is_Little());
    bool l_e=endian.is_Little(), b_e=endian.is_Big();
    assert(  l_e!=b_e );
    char const* which[2]={"little","big"};
    cout<< which[b_e]  << endl;
    return b_e;

