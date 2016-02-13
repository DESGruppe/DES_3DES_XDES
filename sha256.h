#ifndef SHA256_H
#define SHA256_H

/*Header-Files*/
#include <stddef.h>


#define SHA256_BLOCK_SIZE 32            // SHA256 outputs sind 32 byte digest

/*Datentypen*/
typedef unsigned int  WORD;             // 32-bit word, aendern zu "long" fuer 16-bit machines

typedef struct {
	unsigned char data[64];
	WORD datalen;
	unsigned long long bitlen;
	WORD state[8];
} SHA256_CTX;

/*Functiondeklaration*/
void sha256_init(SHA256_CTX *ctx);
void sha256_update(SHA256_CTX *ctx, const unsigned char data[], size_t len);
void sha256_final(SHA256_CTX *ctx, unsigned char hash[]);

#endif   // SHA256_H