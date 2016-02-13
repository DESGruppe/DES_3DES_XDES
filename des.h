#ifndef _DES_H_
#define _DES_H_

#define ENCRYPTION_MODE 1
#define DECRYPTION_MODE 0

typedef struct {
	unsigned char k[8];
	unsigned char c[4];
	unsigned char d[4];
} key_setting;

void DES(unsigned char* message_piece, unsigned char* processed_piece, key_setting* key_sets, int mode);
int rand(void);

#endif