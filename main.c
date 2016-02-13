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

#include <stdio.h>
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
void readkeyfile(int argc, char* argv[]);
void openinputfile(int argc, char* argv[]);
void openoutputfile();
/*
 * 
 */

int main(int argc, char* argv[]) {

	int i;

	if (strcmp (argv[0], "des-enc") == 0){
		if(argc == 3) //prueft ob genug Parameter vorhanden sind.
			{
				readkeyfile(argc, argv);
				openinputfile(argc, argv);
				openoutputfile();
			}
	} else if(strcmp (argv[0], "des3-enc") == 0){
		if(argc == 5)
			{
				readkeyfile(argc, argv);
				openinputfile(argc, argv);
				openoutputfile();
			}
	} else if(strcmp (argv[0], "desx-enc") == 0){
		if(argc == 5)
			{
				readkeyfile(argc, argv);
				openinputfile(argc, argv);
				openoutputfile();
			}
	} else if(strcmp (argv[0], "des-dec") == 0){
		if(argc == 3)
			{
				
			}
	} else if(strcmp (argv[0], "des3-dec") == 0){
		if(argc == 5)
			{
				
			}
	} else if(strcmp (argv[0], "desx-dec") == 0){
		if(argc == 5)
			{
				
			}
	} else if(strcmp (argv[0], "ansix9-31-des3") == 0){
		if(argc == 2)
			{
				//getcwd ruft den aktuellen Pfad der C-Datei ab.
				//strcat verbindet den Output von getcwd mit \\symkex.txt.
				//Damit wird eine symkey.txt im Rootpfad der C-Datei erstellt.
				char pfad[256]; //Hier wird der Programmpfad abgespeichert.
				strcat(pfad,"\\symkey.txt");
				key_file = fopen(pfad,"w+b");
				if(!key_file){
					printf("Key-File konnte nicht geöffnet werden!");
					return 1;
				}
				short int bytes_written;
				int bitlen = atoi(argv[1]);

				//Verwendet den aktuellen TimeStamp als Init-Seed für den rand()-Befehl.
				unsigned int init_seed = (unsigned int)time(NULL);
				srand(init_seed);


				unsigned char* symkey = (unsigned char*) malloc(8*sizeof(char));
				generate_key(bitlen, symkey);
				bytes_written = fwrite(symkey, 1, bitlen, key_file);
				if(bytes_written != bitlen){
					printf("Fehler bei der Ausgabe des Keys in das Output-File!");
					fclose(key_file);
					free(symkey);
					return 1;
				}
				free(symkey);
				fclose(key_file);
			}
	} else if(strcmp (argv[0], "sha-256") == 0){
		if(argc == 3)
			{
				
			}
	} else {
		printf("Wrong Command!\npossible commands:\n des-enc symkey input-file\n des3-enc symkey1 symkey2 symkey3 input-file\n desx-enc symkey1 symkey2 symkey3 input-file\n des-dec symkey input-file\n des3-dec symkey1 symkey2 symkey3 input-file\n desx-dec symkey1 symkey2 symkey3 input-file\n ansix9-31-des3 bitlength\n sha-256 input-file");
	}

	return EXIT_SUCCESS;

}

void readkeyfile(int argc, char *argv[]){
	for( int i = 1; i <= argc-1; i++)
	{
		key_file =fopen(argv[i], "rb");
		if(!key_file){
			printf("Key-File konnte nicht geöffnet werden!");
		}

		short int bytes_read[3];
		unsigned char* symkey = (unsigned char*) malloc(8*sizeof(char));
		bytes_read[i-1] = fread(symkey, sizeof(unsigned char), DES_KEY_SIZE,key_file);
		if(bytes_read[i-1] != DES_KEY_SIZE) {
			printf("Der Key hat nicht die benötigte Länge!");
			fclose(key_file);
		}
		fclose(key_file);
	}
}

void openinputfile(int argc, char *argv[]){
	if(argc == 3 || argc == 5)
	{
		input_file = fopen(argv[argc], "rb");
		if (!input_file) {
			printf("Input File konnte nicht ausgelesen werden!");
		}
	}
}

void openoutputfile(){
	char pfad[256]; //Speichert den Pfad von der Output-Datei.
	strcat(pfad,"\\output.tgp"); //Absolute Pfad zur Outputdatei.
	output_file = fopen(pfad,"w+b");
	if(!output_file){
		printf("Output-File konnte nicht geöffnet werden!");
	}
}
