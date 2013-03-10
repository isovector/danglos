#include "bit_vector.h"

void set_bit(bit_vector b, size_t bit, int on)
{
	if(on)
		b[bit / 8] |= 1 << (bit % 8);
	else
		b[bit / 8] &= ~(1 << (bit % 8));
}

char get_bit(bit_vector b, size_t bit)
{
    return b[bit / 8] & (1 << (bit % 8));
}
