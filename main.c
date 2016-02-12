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
#include "des.h"
#include "prng.h"
#include "sha256.h"

/*
 * 
 */
int main(int argc, char** argv) {

	int i;

	switch(argv[0]){
		case "des-enc": 
			if(argv.length() = 3) //prueft ob genug Parameter vorhanden sind.
			{
				des()
			}
			break;
		case "des3-enc":
			if(argv.length() = 5)
			{

			}
			break;
		case "desx-enc": 
			//TO-DO
			break;
		case "des-dec": 
			if(argv.length() = 3)
			{
				
			}
			break;
		case "des3-dec": 
			if(argv.length() = 5)
			{
				
			}
			break;
		case "desx-dec": 
			//TO-DO
			break;
		case "ansix9-31-des3": 
			if(argv.length() = 2)
			{
				
			}
			break;
		case "sha-256": 
			if(argv.length() = 3)
			{
				
			}
			break;
		default:
			put("Wrong Command!\npossible commands:\n des-enc, des3-enc, desx-enc, des-dec, des3-dec, desx-dec, ansix9-31-des3, sha-256")
		break;
	}


	return EXIT_SUCCESS;




/*
	//Hilfsvariable für den User-Input
    char input;
    bool isRunning = true;
    char[] parameter;

    while(isRunning==true)
    {
        //Bereinigt die Console
        system("clear");        //Fuer Unix-based OS, cls für Windows-based

        //KeyBoard-Buffer bereinigen
        fflush(stdin);
        //Auflistung der Möglichkeiten
        puts("\n[des-enc symkey file] DES-Encryption"
             "\n[des3-enc symkey1 symkey2 symkey3 file] 3DES-Encryption"
             "\n[desx-enc ? file] DESX-Encryption"
             "\n[des-dec symkey file] DES-Decryption"
             "\n[des3-dec symkey1 symkey2 symkey3 file] 3DES-Decryption"
             "\n[desx-dec ? file] DESX-Decryption"
             "\n[ansix9-31-des3 bitlength] PRNG aufrufen"
             "\n[sha-256 file] SHA256 Hash generieren"
             //"\n[help] Beschreibung zu den einzelnen Operationen."
             "\n[x]Exit");
        //Input einlesen
        while( input = getchar()) != ' ')
        //input = getchar();

        switch(option)
        {
            case 'des-enc':
            		 parameter = Parameter_einlesen();
            		 des()
                     break;
            case 'des3-enc':
                     //TO DO CODE
                     break;
            case 'desx-enc':
                     //TO DO CODE
                     break;
            case 'des-dec':
                     //TO DO CODE
                     break;
            case 'des3-dec':
                     //TO DO CODE
                     break;
            case 'desx-dec':
                     //TO DO CODE
                     break;
            case 'ansix9-31-des3':
                     //TO DO CODE
                     break;
            case 'sha-256':
                     //TO DO CODE
                     break;
            case 'help':
                     //TO DO CODE
                     break;
            //...
            case 'x':
                     //Exits the system
                     isRunning = false;
                     return 0;
            default :
                     //User enters wrong input
                     puts("falscher Input!")
                     break;
        }
    }
    return 0;
*/
}

