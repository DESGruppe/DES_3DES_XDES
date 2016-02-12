#ifndef PRNG_H
#define PRNG_H

typedef struct {
	unsigned char k[8];
	unsigned char c[4];
	unsigned char d[4];
} key_set;

void generate_key(unsigned char* key);
void generate_sub_keys(unsigned char* main_key, key_set* key_sets);
#endif


