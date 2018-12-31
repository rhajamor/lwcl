#include<iostream>
#include<bitset>
#include<sstream>
#include <type_traits>
#include <initializer_list>
/* a=target variable, b=bit number to act upon 0-n */
#define BIT_SET(a,b) ((a) |= (1ULL<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1ULL<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1ULL<<(b)))
#define BIT_CHECK(a,b) (!!((a) & (1ULL<<(b))))        // '!!' to make sure this returns 0 or 1

/* x=target variable, y=mask */
#define BITMASK_SET(x,y) ((x) |= (y))
#define BITMASK_CLEAR(x,y) ((x) &= (~(y)))
#define BITMASK_FLIP(x,y) ((x) ^= (y))
#define BITMASK_CHECK_ALL(x,y) (((x) & (y)) == (y))   // warning: evaluates y twice
#define BITMASK_CHECK_ANY(x,y) ((x) & (y))

template<typename T>
static std::string toBinaryString(const T& x)
{
    std::stringstream ss;
    ss << std::bitset<sizeof(T) * 8>(x);
    return ss.str();
}

template <typename uint_t = uint8_t>
struct data_value
{

data_value()=default;
~data_value()=default;
data_value(uint_t _value):octet(_value){}

template<uint_t bit>
inline typename std::enable_if<((sizeof(uint_t)*8)>bit)>::type  clear_n ()
{
  octet &=~ (1U << bit);
}

template<uint_t bit, uint8_t value>
inline typename std::enable_if< ((sizeof(uint_t)*8)>bit )&&(value==0||value==1) >::type set_n ()
{
  uint_t newbit = !!value;    // Also booleanize to force 0 or 1
  octet ^= (-newbit ^ octet) & (1UL << bit);
}

template<uint_t bit>
inline typename std::enable_if<((sizeof(uint_t)*8) > bit) >::type activate_n ()
{
  octet |= 1UL << bit;
}

template<uint_t bit>
inline typename std::enable_if<((sizeof(uint_t)*8) > bit ) , bool >::type is_active ()
{
 return (octet >> bit ) & 1UL;
}

template<uint_t bit>
inline  typename std::enable_if<((sizeof(uint_t)*8)>bit) >::type toggle_n ()
{
	octet ^= 1UL << bit;
}

 inline uint_t get()
 {
	 return octet;
 }
private:
 uint_t octet;


// a) identifier octets (see 8.1.2);
// b) length octets (see 8.1.3);
// c) contents octets (see 8.1.4);
// d) end-of-contents octets (see 8.1.5).
};

int main()
{
	
	uint8_t x=32;
	uint8_t tag=x&0x20;
	std::cout << std::dec << (uint32_t)tag <<"=" << std::hex << (uint32_t)tag
		<< "  (tag==0x1f) " << std::boolalpha <<(tag==0x1f)<< "="<<toBinaryString( tag) << "   "
		<<toBinaryString( (uint8_t)(tag >> 5))<<"\n";
	
	data_value<uint8_t> d{x};
	std::cout << "binary="<<toBinaryString( d.get()) <<  " bit 8 is active ?="<<std::boolalpha<< d.is_active<7>()<<"\n";
	d.activate_n<3>();
	std::cout << "binary="<<toBinaryString( d.get()) <<  " bit 4 is active ?="<<std::boolalpha<< d.is_active<3>()<<"\n";
	d.toggle_n<0>();
	std::cout << "binary="<<toBinaryString( d.get()) <<  " bit 0 is active ?="<<std::boolalpha<< d.is_active<0>()<<"\n";
	d.clear_n<0>();
	std::cout << "binary="<<toBinaryString( d.get()) <<  " bit 0 is active ?="<<std::boolalpha<< d.is_active<0>()<<"\n";
	d.set_n<3,0>();
	d.set_n<7,1>();
	std::cout << "binary="<<toBinaryString( d.get()) <<  " bit 8 is active ?="<<std::boolalpha<< d.is_active<7>()<<"\n";

	
	
	std::bitset<8> bits;

	std::cout << "binary=" <<bits<<"\n";
	
	//BIT_SET(x,6);
	bits=0xff;
	std::cout << "binary=" <<bits << " bit 8 is active ?="<<std::boolalpha<< bits.test(7)<<"\n";
	bits[7]=0;
	std::cout << "binary=" <<bits << " bit 8 is active ?="<<std::boolalpha<< bits.test(7)<<"\n";

	return 0;
}