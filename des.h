#ifndef _DES_H_
#define _DES_H_

#include "prng.h"
#define ENCRYPTION_MODE 1
#define DECRYPTION_MODE 0

void process_message(unsigned char* message_piece, unsigned char* processed_piece, key_set* key_sets, int mode);
int rand(void);

#endif