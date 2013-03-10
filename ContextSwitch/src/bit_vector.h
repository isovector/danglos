#ifndef _BIT_VECTOR_H_
#define _BIT_VECTOR_H_

typedef char * bit_vector;

void set_bit(bit_vector b, size_t bit, int on);
char get_bit(bit_vector b, size_t bit);

#endif//_BIT_VECTOR_H_
