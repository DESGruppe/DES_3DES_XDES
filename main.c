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


//Definition der file-Handler
 static FILE *key_file, *input_file, *output_file;

//DES-Key Länge
 #define DES_KEY_SIZE 8

//Declaration der Functions
void read_key_file(int argc, char* argv[], int cip);
void open_input_file(int argc, char* argv[]);
void open_output_file();
void generate_DES_key_set (short int process_mode);
/*
 * 
 */
unsigned char* symkey;

int main(int argc, char* argv[]) {
	
	symkey = (unsigned char*) malloc(8*sizeof(char));

	//Zu jeden DES gehoert noch eine Base64 codierung.
	if (strcmp (argv[1], "des-enc") == 0){
		if(argc == 4) //prueft ob genug Parameter vorhanden sind.
			{
				read_key_file(argc, argv, 2);
				open_input_file(argc, argv);
				open_output_file();
				generate_DES_key_set(ENCRYPTION_MODE);

			}
	} else if(strcmp (argv[1], "des3-enc") == 0){
		if(argc == 6)
			{
				read_key_file(argc, argv, 2);
				open_input_file(argc, argv);
				open_output_file();
				generate_DES_key_set(ENCRYPTION_MODE);
				read_key_file(argc, argv, 3);
				open_input_file(argc, argv);
				open_output_file();
				generate_DES_key_set(DECRYPTION_MODE);
				read_key_file(argc, argv, 4);
				open_input_file(argc, argv);
				open_output_file();
				generate_DES_key_set(ENCRYPTION_MODE);
			}
	} else if(strcmp (argv[1], "desx-enc") == 0){
		if(argc == 6)
			{
				//TO-DO
			}
	} else if(strcmp (argv[1], "des-dec") == 0){
		if(argc == 4)
			{
				read_key_file(argc, argv, 1);
				open_input_file(argc, argv);
				open_output_file();
				generate_DES_key_set(DECRYPTION_MODE);	
			}
	} else if(strcmp (argv[1], "des3-dec") == 0){
		if(argc == 6)
			{
				read_key_file(argc, argv, 2);
				open_input_file(argc, argv);
				open_output_file();
				generate_DES_key_set(DECRYPTION_MODE);
				read_key_file(argc, argv, 3);
				open_input_file(argc, argv);
				open_output_file();
				generate_DES_key_set(ENCRYPTION_MODE);
				read_key_file(argc, argv, 4);
				open_input_file(argc, argv);
				open_output_file();
				generate_DES_key_set(DECRYPTION_MODE);
			}
	} else if(strcmp (argv[1], "desx-dec") == 0){
		if(argc == 6)
			{
				//TO-DO	
			}

	} else if(strcmp (argv[1], "ansix9-31-des3") == 0){ //Base64 Codierung gehoert noch implementiert.
		if(argc == 3)
			{
				key_file = fopen("symkey","w+b");
				if(!key_file){
					printf("Key-File konnte nicht geöffnet werden!");
					return 1;
				}
				short int bytes_written;
				int bitlen = atoi(argv[2]);
				printf("%i", bitlen);
				//Verwendet den aktuellen TimeStamp als Init-Seed für den rand()-Befehl.
				srand(time(NULL));

				generate_key(bitlen, symkey);
				bytes_written = fwrite(symkey, sizeof(char), bitlen, key_file);
				if(bytes_written != bitlen){
					printf("Fehler bei der Ausgabe des Keys in das Output-File!");
					fclose(key_file);
					free(symkey);
					return 1;
				}
				free(symkey);
				fclose(key_file);
			}
	} else if(strcmp (argv[1], "sha-256") == 0){
		if(argc == 3)
			{
				unsigned char buf[SHA256_BLOCK_SIZE];
				unsigned char* file;
				SHA256_CTX ctx;
				open_input_file(argc, argv);
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
		printf("Wrong Command!\npossible commands:\n des-enc symkey input-file\n des3-enc symkey1 symkey2 symkey3 input-file\n desx-enc symkey1 symkey2 symkey3 input-file\n des-dec symkey input-file\n des3-dec symkey1 symkey2 symkey3 input-file\n desx-dec symkey1 symkey2 symkey3 input-file\n ansix9-31-des3 bitlength\n sha-256 input-file\n");
	}

	return EXIT_SUCCESS;

}

//Liest die symkeys aus, egal ob des, des3 oder desx
void read_key_file(int argc, char *argv[],int cip){
		key_file =fopen(argv[cip], "rb");
		if(!key_file){
			printf("Key File konnte nicht geöffnet werden!");
		}

		short int bytes_read;
		//unsigned char* symkey = (unsigned char*) malloc(8*sizeof(char));
		bytes_read = fread(symkey, sizeof(unsigned char), DES_KEY_SIZE, key_file);
		if(bytes_read != DES_KEY_SIZE) {
			printf("Der Key hat nicht die benötigte Länge!");
			fclose(key_file);
		}
		fclose(key_file);
	}

void open_input_file(int argc, char *argv[]){
		input_file = fopen(argv[argc-1], "rb");
		if (!input_file) {
			printf("Input File konnte nicht ausgelesen werden!");
		}
}

void open_output_file(){
	output_file = fopen("output.tgp","w+b");
	if(!output_file){
		printf("Output File konnte nicht geöffnet werden!");
	}
}

void generate_DES_key_set (short int process_mode){
	short int bytes_written;
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
	number_of_blocks = file_size/8 + ((file_size%8)?1:0);

	//Start fuer das auslesen der Input file und das schreiben der Output file.
	while(fread(data_block, 1, 8, input_file)){
		block_count++;
		if(block_count == number_of_blocks){
			if(process_mode == ENCRYPTION_MODE){
				padding = 8- file_size%8;
				if(padding < 8){ //Befuellt leere Bloecke mit Padding.
					memset((data_block +8 - padding), (unsigned char)padding, padding);
				}

				process_message(data_block, processed_block, key_sets, process_mode);
				bytes_written = fwrite(processed_block, 1, 8, output_file);

				if (padding == 8) { //Extra block fuer padding
					memset(data_block, (unsigned char)padding, 8);
					process_message(data_block, processed_block, key_sets, process_mode);
					bytes_written = fwrite(processed_block, 1, 8, output_file);
				}
			} else if( process_mode == DECRYPTION_MODE) {
					process_message(data_block, processed_block, key_sets, process_mode);
					padding = processed_block[7];

					if (padding < 8){
						bytes_written= fwrite(processed_block, 1, 8 - padding, output_file);
					}
				} else{
					printf("Falscher Modus angegeben!");
				}
			} else{
				process_message(data_block, processed_block, key_sets, process_mode);
				bytes_written = fwrite(processed_block, 1, 8, output_file);
			}
			memset(data_block, 0, 8);
		}

			//Speicher entleeren
			free(symkey);
			free(data_block);
			free(processed_block);
			fclose(input_file);
			fclose(output_file);
}
