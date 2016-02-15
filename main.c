/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: des_gruppe
 *
 * Created on 02. Jänner 2016, 16:41
 */

#include <stdio.h> // File
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "des.h"
#include "prng.h"
#include "sha256.h"
#include "base64.h"


//Definition der file-Handler
 static FILE *key_file, *input_file, *output_file;

//DES-Key Länge
 #define DES_KEY_SIZE 8

//Declaration der Functions
void read_key_file(int argc, char* argv[], int cip);
void open_input_file(char* inputfile);
void open_output_file(char* outputfile);
void generate_DES_key_set (short int process_mode);
void base64_encoding(unsigned char *tmp, FILE *destination);
void base64_decoding(unsigned char *tmp, FILE *destination);
/*
 * 
 */
unsigned char* symkey;

int main(int argc, char* argv[]) {

	unsigned char* tmp;
	symkey = (unsigned char*) malloc(8*sizeof(char));

	//Diese IF-Verzweigungen prüfen, welcher Befehl eingegeben wurde.
	//DES encryption
	if (strcmp (argv[1], "des-enc") == 0){ //Erledigt
		if(argc == 5) //prueft ob genug Parameter vorhanden sind.
			{

				//Encryption
				read_key_file(argc, argv, 2);
				open_input_file(argv[argc-2]);
				open_output_file("encrypt.tgp");
				generate_DES_key_set(ENCRYPTION_MODE);

				//BASE64-Encodierung
				open_input_file("encrypt.tgp");
				open_output_file(argv[argc-1]);
				//File-size berechnen.
				fseek(input_file, 0L, SEEK_END);
				size_t file_size = ftell(input_file);
				fseek(input_file, 0L, SEEK_SET);
				//Die benötigte Größe von tmp setzen. +2 wegen \0.
				tmp = malloc(file_size+2);
				//Liest die Input-Datei aus und speichert diese in tmp ab.
				fread(tmp, sizeof(unsigned char), file_size, input_file);
				//Ruft die Funktion in der main.c auf.
				base64_encoding(tmp, output_file);
				free(tmp);
				fclose(input_file);
				fclose(output_file);


			}
	//3DES encryption
	} else if(strcmp (argv[1], "des3-enc") == 0){
		if(argc == 7)
			{
				read_key_file(argc, argv, 2);
				open_input_file(argv[argc-2]);
				open_output_file(argv[argc-1]);
				generate_DES_key_set(ENCRYPTION_MODE);
				
				read_key_file(argc, argv, 3);
				open_input_file(argv[argc-1]);
				open_output_file("decrypt3des.tgp");
				generate_DES_key_set(DECRYPTION_MODE);

				read_key_file(argc, argv, 4);
				open_input_file("decrypt3des.tgp");
				open_output_file("encrypt3des.tgp");
				generate_DES_key_set(ENCRYPTION_MODE);

				//BASE64-Codierung
				open_input_file("encrypt3des.tgp");
				open_output_file(argv[argc-1]);
				//File-size aufrufen.
				fseek(input_file, 0L, SEEK_END);
				size_t file_size = ftell(input_file);
				fseek(input_file, 0L, SEEK_SET);

				tmp = malloc(file_size+2);
				fread(tmp, sizeof(unsigned char), file_size, input_file);
				base64_encoding(tmp, output_file);
				free(tmp);
				fclose(input_file);
				fclose(output_file);
			}
	//DESX encryption
	} else if(strcmp (argv[1], "desx-enc") == 0){
		if(argc == 7)
			{
				unsigned char* help;
				size_t file_size;
				//Pre-Whitening
				read_key_file(argc,argv, 2);
				open_input_file(argv[argc-2]);
				open_output_file(argv[argc-1]);
				fseek(input_file, 0L, SEEK_END);
				file_size = ftell(input_file);
				fseek(input_file, 0L, SEEK_SET);
				tmp = malloc(file_size);
				fread(tmp, sizeof(unsigned char), file_size, input_file);
				//printf("tmp: %s\n, symkey: %s\n, längen: %li, %li, %li\n",tmp,symkey,file_size,sizeof(tmp),sizeof(symkey));
				help = malloc(file_size);
				for(int j = 0; j < file_size; j += 8)
				{
					for (int i = 0; i < sizeof(symkey); i++){
						help[i+j] = tmp[i+j] ^ symkey[i];
					}
				}
				/*printf("%s\n",help);
				for(int i = 0; i < sizeof(symkey) ; i++){
					printf("%c\n", symkey[i]);
				}*/

				fprintf(output_file, "%s", help);
				printf("%s\n",help);
				free(tmp);
				free(help);
				fclose(input_file);
				fclose(output_file);

				//DES-Encryption
				read_key_file(argc, argv, 3);
				open_input_file(argv[argc-1]);
				open_output_file("encryptx.tgp");
				generate_DES_key_set(ENCRYPTION_MODE);


				//Post-Whitening
				read_key_file(argc,argv, 4);
				open_input_file("encryptx.tgp");
				open_output_file("postwhitening.tgp");
				fseek(input_file, 0L, SEEK_END);
				file_size = ftell(input_file);
				fseek(input_file, 0L, SEEK_SET);
				tmp= malloc(file_size);
				fread(tmp, sizeof(unsigned char), file_size, input_file);
				help=malloc(file_size);

				for(int j = 0; j < file_size; j += 8)
				{
					for (int i = 0; i < sizeof(symkey); i++){
						help[i+j] = tmp[i+j] ^ symkey[i];
					}
				}

				fprintf(output_file, "%s", help);
				free(tmp);
				free(help);
				fclose(input_file);
				fclose(output_file);

				//BASE64-Encodierung
				open_input_file("postwhitening.tgp");
				open_output_file(argv[argc-1]);
				//File-size berechnen.
				fseek(input_file, 0L, SEEK_END);
				file_size = ftell(input_file);
				fseek(input_file, 0L, SEEK_SET);
				//Die benötigte Größe von tmp setzen. +2 wegen \0.
				tmp = malloc(file_size);
				//Liest die Input-Datei aus und speichert diese in tmp ab.
				fread(tmp, sizeof(unsigned char), file_size, input_file);
				//Ruft die Funktion in der main.c auf.
				base64_encoding(tmp, output_file);
				free(tmp);
					
				fclose(input_file);
				fclose(output_file);
			}
	//DES decryption
	} else if(strcmp (argv[1], "des-dec") == 0){ //Erledigt
		if(argc == 5)
			{
				//BASE64-Codierung
				open_input_file(argv[argc-2]);
				open_output_file("base64decode.tgp");
				//File-Size abrufen.
				fseek(input_file, 0L, SEEK_END);
				size_t file_size = ftell(input_file);
				fseek(input_file, 0L, SEEK_SET);

				tmp = malloc(file_size+2);
				fread(tmp, sizeof(unsigned char), file_size, input_file);
				base64_decoding(tmp, output_file);
				free(tmp);
				fclose(input_file);
				fclose(output_file);

				read_key_file(argc, argv, 2);
				open_input_file("base64decode.tgp");
				open_output_file(argv[argc-1]);
				generate_DES_key_set(DECRYPTION_MODE);
			}
	//3DES decryption
	} else if(strcmp (argv[1], "des3-dec") == 0){
		if(argc == 7)
			{
				
				//BASE64-Codierung
				open_input_file(argv[argc-2]);
				open_output_file(argv[argc-1]);
				//File-size aufrufen.
				fseek(input_file, 0L, SEEK_END);
				size_t file_size = ftell(input_file);
				fseek(input_file, 0L, SEEK_SET);

				tmp = malloc(file_size);
				fread(tmp, sizeof(unsigned char), file_size, input_file);
				base64_decoding(tmp, output_file);
				free(tmp);
				fclose(input_file);
				fclose(output_file);

				//Decryption
				read_key_file(argc, argv, 2);
				open_input_file(argv[argc-1]);
				open_output_file("decrypt3des.tgp");
				generate_DES_key_set(DECRYPTION_MODE);

				read_key_file(argc, argv, 3);
				open_input_file("decrypt3des.tgp");
				open_output_file("encrypt3des.tgp");
				generate_DES_key_set(ENCRYPTION_MODE);

				read_key_file(argc, argv, 4);
				open_input_file("encrypt3des.tgp");
				open_output_file(argv[argc-1]);
				generate_DES_key_set(DECRYPTION_MODE);
			}
	//DESX decryption
	} else if(strcmp (argv[1], "desx-dec") == 0){
		if(argc == 7)
			{
				unsigned char* help;
				size_t file_size;

				//BASE64-Codierung
				open_input_file(argv[argc-2]);
				open_output_file("base64decode.tgp");
				//File-Size abrufen.
				fseek(input_file, 0L, SEEK_END);
				file_size = ftell(input_file);
				fseek(input_file, 0L, SEEK_SET);
				tmp = malloc(file_size);
				fread(tmp, sizeof(unsigned char), file_size, input_file);
				base64_decoding(tmp, output_file);
				free(tmp);
				fclose(input_file);
				fclose(output_file);


				//Post-Whitening
				read_key_file(argc,argv, 2);
				open_input_file("base64decode.tgp");
				open_output_file(argv[argc-1]);
				fseek(input_file, 0L, SEEK_END);
				file_size = ftell(input_file);
				fseek(input_file, 0L, SEEK_SET);
				tmp = malloc(file_size);
				fread(tmp, sizeof(unsigned char), file_size, input_file);
				help = malloc(file_size);
				for(int j = 0; j < file_size; j += 8)
				{
					for (int i = 0; i < sizeof(symkey); i++){
						help[i+j] = tmp[i+j] ^ symkey[i];
					}
				}
				fprintf(output_file, "%s", help);
				free(tmp);
				free(help);
				fclose(input_file);
				fclose(output_file);

				//DES-Encryption
				read_key_file(argc, argv, 3);
				open_input_file(argv[argc-1]);
				open_output_file("decryptx.tgp");
				generate_DES_key_set(DECRYPTION_MODE);


				//Pre-Whitening
				read_key_file(argc,argv, 4);
				open_input_file("decryptx.tgp");
				open_output_file(argv[argc-1]);
				fseek(input_file, 0L, SEEK_END);
				file_size = ftell(input_file);
				fseek(input_file, 0L, SEEK_SET);
				tmp = malloc(file_size);
				fread(tmp, sizeof(unsigned char), file_size, input_file);
				help=malloc(file_size);
				for(int j = 0; j < file_size; j += 8)
				{
					for (int i = 0; i < sizeof(symkey); i++){
						help[i+j] = tmp[i+j] ^ symkey[i];
					}
				}
				fprintf(output_file, "%s", help);
				free(tmp);
				free(help);
				fclose(input_file);
				fclose(output_file);	
			}
	//PRNG
	} else if(strcmp (argv[1], "ansix9-31-des3") == 0){ //Erledigt.
		if(argc == 3)
			{
				key_file = fopen("symkey","w+b");
				if(!key_file){
					printf("Key-File konnte nicht geöffnet werden!");
					return 1;
				}
				//atoi wandelt String in Int um. (verwendet nicht den Hex-Wert des Strings)
				size_t bitlen = atoi(argv[2]);

				//Verwendet den aktuellen TimeStamp als Init-Seed für den rand()-Befehl.
				srand(time(NULL));

				generate_key(bitlen, symkey);
				//unsigned char test[] = "Hallo mein Name ist Alexander!";
				//size_t bitlen = sizeof(test);
				//symkey= test;
				base64_encoding(symkey, key_file);
				free(symkey);
				fclose(key_file);
			}
	//SHA256
	} else if(strcmp (argv[1], "sha-256") == 0){ //Erledigt.
		if(argc == 3)
			{
				unsigned char buf[SHA256_BLOCK_SIZE];
				unsigned char* file;
				SHA256_CTX ctx;
				open_input_file(argv[argc-1]);
				fseek(input_file, 0L, SEEK_END);
				size_t file_size = ftell(input_file);
				fseek(input_file, 0L, SEEK_SET);
				file = malloc(sizeof(unsigned char)*file_size);
				fread(file, sizeof(unsigned char), file_size, input_file);
				sha256_init(&ctx);
				sha256_update(&ctx, file, file_size);
				sha256_final(&ctx, buf);
				for(int j=0;j<sizeof(buf);j++){
					fprintf(stdout, "%.2x" , buf[j]);
				}
				fprintf(stdout, "\n");
				fclose(input_file);
			}
	} else {
		printf("Wrong Command!\npossible commands:\n des-enc symkey input-file output-file\n");
		printf("des3-enc symkey1 symkey2 symkey3 input-file output-file\n desx-enc symkey1 symkey2 symkey3 input-file output-file\n");
		printf("des-dec symkey input-file output-file\n des3-dec symkey1 symkey2 symkey3 input-file output-file\n ");
		printf("desx-dec symkey1 symkey2 symkey3 input-file output-file\n ansix9-31-des3 bitlength\n sha-256 input-file\n");
	}

	return EXIT_SUCCESS;

}

/*
*	Liest die Key-File für die diversen DES-Verfahren aus.
*/
void read_key_file(int argc, char *argv[],int cip){
		
		short int bytes_read;
		//In cip ist der Index der benötigten Key-File gespeichert.
		key_file = fopen(argv[cip], "rb");
		if(!key_file){
			printf("Key File konnte nicht geöffnet werden!");
		}
		//Liest den Key aus der File und speichert diesen in symkey ab.
		bytes_read = fread(symkey, sizeof(unsigned char), DES_KEY_SIZE, key_file);
		//Wenn der Key nicht die benötigte Länge von 8 Byte hat, wird eine Meldung ausgegeben und die der File-Handler geschlossen.
		if(bytes_read != DES_KEY_SIZE) {
			printf("Der Key hat nicht die benötigte Länge!");
			fclose(key_file);
		}
		fclose(key_file);
	}
/*
*	Öffnet die übergebene Input-Datei und gibt eine Fehlermeldung aus, wenn diese nicht existiert.
*/
void open_input_file(char* inputfile){
		input_file = fopen(inputfile, "rb");
		if (!input_file) {
			printf("Input File konnte nicht ausgelesen werden!");
		}
}

/*
*	Öffnet die übergebene Output-Datei und falls diese noch nicht exisitert
*	wird diese automatisch erstellt.
*/
void open_output_file(char* outputfile){
	output_file = fopen(outputfile,"w+b");
	if(!output_file){
		printf("Output File konnte nicht geöffnet werden!");
	}
}

/*
*	Setzt die Vorbereitungen für den Base64-Encoder.
*	Ruft die base64_encode Funktion in der base64.c Datei auf.
*/
void base64_encoding(unsigned char *tmp, FILE *destination){
	unsigned int bytes_written;
	size_t length=0;
	//Berechnet wie lang der Input ist.
	for(int i = 0; strcmp((const char*)tmp+i, "\0"); i++) {
    	length ++;
	}
	//Encoded die tmp Datei in ein Base64-Format.
	//Der Rückgabe-Parameter wird wieder in tmp gespeichert und die neue Länge der BASE64-Kette wird in length gespeichert.
	printf("length: %li\n",length);
	tmp = base64_encode(tmp, length, &length);
	//Schreibt die Base64-Kette in die übergebene Output-Datei.
	bytes_written = fprintf(destination, "%s", tmp);
	printf("length: %li, written: %i\n",length, bytes_written);
	if(bytes_written+1 != length){
		printf("Fehler bei der Ausgabe in Base64!");
	}
	free(tmp);
}

/*
*	Setzt die Vorbereitungen für Base64-Decoding.
*	Ruft die base64_decode Funktion in der base64.c Datei auf.
*/
void base64_decoding(unsigned char *tmp, FILE *destination){
	unsigned int bytes_written;
	size_t length=0;
	
	//Berechnet wie lang der Input ist.
	for(int i = 0; strcmp((const char*)tmp+i, "\0"); i++) {
    	length ++;
	}
	//Decodiert die tmp Datei, length sagt wie viele Zeichen in tmp vorhanden sind
	//Der Rückgabe Parameter wird wieder in tmp gespeichert und in length wird die Länge der ASCII-Kette gespeichert.
	tmp = base64_decode(tmp, length, &length);
	//Schreibt die ASCII-Zeichen in die übergebene Datei.
	bytes_written = fprintf(destination, "%s", tmp);
	printf("length: %li, written: %i\n",length, bytes_written);

	if(bytes_written != length){
		printf("Fehler bei der Ausgabe in ASCII!");
	}
	free(tmp);
}

void generate_DES_key_set (short int process_mode){
	unsigned long block_count = 0, number_of_blocks, file_size;
	unsigned char* data_block = (unsigned char*) malloc(8*sizeof(char));
	unsigned char* processed_block = (unsigned char*) malloc(8*sizeof(char));
	unsigned short int padding;
	key_set* key_sets = (key_set*) malloc(17*sizeof(key_set));

	generate_sub_keys(symkey,key_sets);

	//Gibt die Anzahl der Blocks im File zurueck.
	fseek(input_file, 0L, SEEK_END);
	file_size = ftell(input_file);

	fseek(input_file, 0L, SEEK_SET);
	//Unterteilung in 8 Bit Blöcken.
	number_of_blocks = file_size/8 + ((file_size%8)?1:0);

	//Start fuer das auslesen der Input file und das schreiben der Output file.
	while(fread(data_block, 1, 8, input_file)){
		block_count++;
		//Entscheidet ob gepaddet gehört oder nicht. Tritt nur beim letzten 8*8 Byte-Block auf.
		if(block_count == number_of_blocks){
			//Prueft welcher Modus aktiviert ist. (Encrypt oder Decrypt)
			if(process_mode == ENCRYPTION_MODE){
				printf("Entering Padding!\n");
				//Berechnet die Anzahl der zu paddenden Felder.
				padding = 8 - file_size%8;
				if(padding < 8){
					//Springt an das erste zu paddende Feld und schreibt die Anzahl der zu paddenden Felder in die gepaddeden Felder.
					memset((data_block + 8 - padding), padding, padding);
				}
				//Ruft die process_message Funktion in des.c auf.
				process_message(data_block, processed_block, key_sets, process_mode);
				//Schreibt das Ergebnis in die output-File.
				fwrite(processed_block, 1, 8, output_file);
				if (padding == 8) { //Schreibt einen extra Block für Padding.
					memset(data_block, padding, 8);
					process_message(data_block, processed_block, key_sets, process_mode);
					fwrite(processed_block, 1, 8, output_file);
				}
			//Padding entfernen beim Decryption.
			} else {
					printf("Entering Padding-Decryption!\n");
					process_message(data_block, processed_block, key_sets, process_mode);
					padding = processed_block[7];
					printf("%hu\n", padding);
					if (padding < 8){
						fwrite(processed_block, 1, 8 - padding, output_file);
					}
			}
			//Wenn nicht gepaddet gehört, dann nur Verschlüsselung.
		} else{
			printf("Entering non-padding %i!\n", process_mode);
			process_message(data_block, processed_block, key_sets, process_mode);
			fwrite(processed_block, 1, 8, output_file);
		}
		memset(data_block, 0, 8);
	}

	//Speicher entleeren
	free(data_block);
	free(processed_block);
	fclose(input_file);
	fclose(output_file);
}
