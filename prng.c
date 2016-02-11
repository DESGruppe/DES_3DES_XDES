void generate_key(unsigned char* key){
	int i;
	for (i=0; i<8; i++) {
		key[i] = rand()%255;
	}
}


void generate_sub_keys(unsigned char* main_key, key_set* key_sets){
	int i, j;
	int shift_size;
	unsigned char shift_byte, first_shift_bits, second_shift_bits, third_shift_bits, fourth_shift_bits;

	for (i=0; i<8; i++) {
		key_sets[0].k[i] = 0;
	}

	//Initial Permutation (siehe DES-Netzwerk)
	for (i=0; i<56; i++) {
		shift_size = PC1[i];
		//Nach jedem 8 Bit wird es um 1 nach Rechts geshiftet.
		shift_byte = 0x80 >> ((shift_size - 1)%8);
		//UND-Verknuepfung mit den Stellen 7 bis 0.
		shift_byte &= main_key[(shift_size - 1)/8];
		//Wird wieder zurueck geshiftet.
		shift_byte <<= ((shift_size - 1)%8);
		//Ersten 8 Durchgänge werden in k[0] gespeichert, Durchgang 9-16 in k[1].
		key_sets[0].k[i/8] |= (shift_byte >> i%8);
	}

	//Ersten 3 Werte von k werden in c abgespeichert.
	for (i=0; i<3; i++) {
		key_sets[0].c[i] = key_sets[0].k[i];
	}

	//Die oberen 4 Bits vom 4. Wert/Index 3 werden zu c dazu genommen.
	key_sets[0].c[3] = key_sets[0].k[3] & 0xF0;


	for (i=0; i<3; i++) {
		//Nimmt die letzte 4 Bits von dem Wert aus k[i+3] und shiftet diese um 4 nach links.
		key_sets[0].d[i] = (key_sets[0].k[i+3] & 0x0F) << 4;
		//Nimmt die ersten 4 Bits von dem Wert aus k[i+4] und ODER-Verknuepft diese mit den Ergebnis aus Zeile darüber.
		key_sets[0].d[i] |= (key_sets[0].k[i+4] & 0xF0) >> 4;
	}
	//Die unteren 4 Bits vom 7. Wert werden zu d dazu genommen.
	key_sets[0].d[3] = (key_sets[0].k[6] & 0x0F) << 4;


	for (i=1; i<17; i++) {
		//In allen key_sets Felder werden die gleichen Werte abgespeichert. (c und d bleiben gleich)
		for (j=0; j<4; j++) {
			key_sets[i].c[j] = key_sets[i-1].c[j];
			key_sets[i].d[j] = key_sets[i-1].d[j];
		}

		shift_size = left_shifts[i];
		if (shift_size == 1){
			shift_byte = 0x80;
		} else {
			shift_byte = 0xC0;
		}

		// Verarbeitung vom C-Array
		first_shift_bits = shift_byte & key_sets[i].c[0];
		second_shift_bits = shift_byte & key_sets[i].c[1];
		third_shift_bits = shift_byte & key_sets[i].c[2];
		fourth_shift_bits = shift_byte & key_sets[i].c[3];

		//Beim ersten Durchgang wird nichts verändert und ab dem 2. Durchgang:
		//Wird das höchste Bit von c verworfen und das niedrigste Bit von c ersetzt.
		key_sets[i].c[0] <<= shift_size;
		key_sets[i].c[0] |= (second_shift_bits >> (8 - shift_size));

		key_sets[i].c[1] <<= shift_size;
		key_sets[i].c[1] |= (third_shift_bits >> (8 - shift_size));

		key_sets[i].c[2] <<= shift_size;
		key_sets[i].c[2] |= (fourth_shift_bits >> (8 - shift_size));

		key_sets[i].c[3] <<= shift_size;
		key_sets[i].c[3] |= (first_shift_bits >> (4 - shift_size));

		// Verarbeitung vom D-Array, sehe Verarbeitung von C
		first_shift_bits = shift_byte & key_sets[i].d[0];
		second_shift_bits = shift_byte & key_sets[i].d[1];
		third_shift_bits = shift_byte & key_sets[i].d[2];
		fourth_shift_bits = shift_byte & key_sets[i].d[3];

		key_sets[i].d[0] <<= shift_size;
		key_sets[i].d[0] |= (second_shift_bits >> (8 - shift_size));

		key_sets[i].d[1] <<= shift_size;
		key_sets[i].d[1] |= (third_shift_bits >> (8 - shift_size));

		key_sets[i].d[2] <<= shift_size;
		key_sets[i].d[2] |= (fourth_shift_bits >> (8 - shift_size));

		key_sets[i].d[3] <<= shift_size;
		key_sets[i].d[3] |= (first_shift_bits >> (4 - shift_size));

		for (j=0; j<48; j++) {
			shift_size = PC2[j];
			//siehe  Initial Permutation-Beschreibung
			if (shift_size <= 28) {
				shift_byte = 0x80 >> ((shift_size - 1)%8);
				shift_byte &= key_sets[i].c[(shift_size - 1)/8];
				shift_byte <<= ((shift_size - 1)%8);
			} else {
				shift_byte = 0x80 >> ((shift_size - 29)%8);
				shift_byte &= key_sets[i].d[(shift_size - 29)/8];
				shift_byte <<= ((shift_size - 29)%8);
			}

			key_sets[i].k[j/8] |= (shift_byte >> j%8);
		}
	}
}