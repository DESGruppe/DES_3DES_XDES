#include <stdlib.h>
#include <string.h>

//ale Zeichen von Base64 ('='' exklusiv, für zeichen die nur aus padding bits bestehen reserviert)
static const unsigned char base64_zeichen[64] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/*
 *	Encodierungs-Funktion ==> ASCII -> Base64
 *
 *	src = Daten zum codieren
 *	len = Größe der Daten zum codieren
 *	out_len = wo Länge des Ouputs hingespeichert werden soll
 *	Rückgabe-Wert = Pointer auf codierte Daten oder NULL-Pointer bei Fehler
 *
 *	out muss vom Caller gefreed werden
 */
unsigned char * base64_encode(const unsigned char *src, size_t len, size_t *out_len) {
	
	unsigned char *out, *pos;
	const unsigned char *end, *in;
	size_t olen;
	int line_len;

	//output-länge berechenen, 3 8 Bit-Blöcke (*3) werden zu 4 6 Bit-Blöcken (/4)
	olen = len * 4 / 3 + 4;
	//alle 72 Zeichen kommt ein Zeilenumbruch rein
	olen += olen / 72;
	//String endet immer mit '\0'
	olen++;
	//Speicher für codierte Daten reservieren
	out = malloc(olen);
	if (out == NULL)
		return NULL;

	//endposition im Array
	end = src + len;
	//anfangsposition im source-array, index-variable
	in = src;
	//anfangsposition im destination-array
	pos = out;
	line_len= 0;

	//solange mehr als 3 8 Bit Blöcke noch nicht abgearbeitet wurde, immer 3 8 Bit Blöcke auf einmal in 4 6 Bit Blöcke umwandeln
	while (end - in >= 3) {
		//höchsten sechs Bit von erstem Block als base64-Zeichen im destination-array abspeichern und pointer ein Feld weiter setzen
		*pos++ = base64_zeichen[in[0] >> 2];
		//niedrigste zwei Bit von erstem Block und höchste 4 Bit von zweitem Block
		*pos++ = base64_zeichen[((in[0] & 0x03) << 4) | (in[1] >> 4)];
		//niedrigste 4 Bit von zweitem Block und hächste zwei Bit von drittem Block
		*pos++ = base64_zeichen[((in[1] & 0x0f) << 2) | (in[2] >> 6)];
		//niedrigste 6 Bit von drittem Block
		*pos++ = base64_zeichen[in[2] & 0x3f];
		//index-variable drei blöcke weiter schieben
		in += 3;
		//anzahl destination-blöcke um 4 erhöhen
		line_len += 4;
		//nach 72 Zeichen wird standard-mäßig ein Zeilenumbruch eingefügt
		if (line_len >= 72) {
			*pos++ = '\n';
			line_len = 0;
		}
	}

	//falls noch 8 Bit Blöcke übrig sind (anzahl ist nicht vielfaches von drei ==> Padding mit nur Nuller notwendig)
	//6 Bit Blöcke die nur aus padding bestehen werden mit '=' befüllt (standard)
	if (end - in) {
		//höchsten sechs Bit von erstem Block
		*pos++ = base64_zeichen[in[0] >> 2];
		//1 8 Bit Block übrig
		if (end - in == 1) {
			//restlichen 2 Bit vom Block als höchste Bit übernehmen
			*pos++ = base64_zeichen[(in[0] & 0x03) << 4];
			//ersten gepaddeten Block erstellen (3. Block der 4-er Kombination)
			*pos++ = '=';
		//2 8 Bit Blöcke übrig
		} else {
			//restliche Bits übernehmen
			*pos++ = base64_zeichen[((in[0] & 0x03) << 4) | (in[1] >> 4)];
			*pos++ = base64_zeichen[(in[1] & 0x0f) << 2];
		}
		//4. Block gepaddeten erstellen
		*pos++ = '=';
		line_len += 4;
	}

	//wenn nicht mit einem Zeilenumbruch geändert wurde
	if (line_len)
		*pos++ = '\n';

	*pos = '\0';
	
	//wenn out_len ein gültiger Pointer ist, die Anzahl an erzeugten Blöcken einspeichern
	//Adresse vonletzter Beschriebener Block - Adresse von erster Block + Null-Terminator
	if (out_len)
		*out_len = pos - out + 1;
	
	return out;
}


/*
 *	Decodierungs-Funktion ==> Base64 -> ASCII
 *
 *	src = Daten zum decodieren
 *	len = Größe der daten zum decodieren
 *	out_len = wo Länge des Ouputs hingespeichert werden soll
 *	Rückgabewert = Pointer auf decodierte Daten oder NULL-Pointer bei Fehler
 *
 *	out muss vom Caller gefreed werden
 */
unsigned char * base64_decode(const unsigned char *src, size_t len, size_t *out_len) {
	
	unsigned char dtable[256], *out, *pos, in[4], block[4], tmp;
	size_t count;

	//schreibt in alle Felder von dtable den Wert 128 (0x80) damit am Ende erkennbar ist, welches Feld nicht verändert wurde
	//Base64 hat 64 Zeichen, in der ASCII-Tabelle alle vor Wert 124 sind ==> 128 oder größer kommt nicht vor -> kann nachher abgefragt werden
	memset(dtable, 0x80, 256);
	//geht alle Zeichen von Base64 durch und befüllt dtable am Index vom ASCII-Wert vom Base64-Zeichen mit dem Index in der Base64-Tabelle vpm Zeichen
	for (int i = 0; i < sizeof(base64_zeichen); i++)
		dtable[base64_zeichen[i]] = i;
	//Index '=' (64) mit Wert 0 befüllen, da '=' das padding mit Nullern war
	dtable['='] = 0;

	//geht alle Zeichen der Source durch, und wenn es ein gültiger Base64-Wert ist wird der Zähler erhöht
	count = 0;
	for (int i = 0; i < len; i++) {
		if (dtable[src[i]] != 0x80)
			count++;
	}

	//wenn die Anzahl der gültigen Zeichen von der Source nicht ein vielfaches von 4 ist -> ungültige Eingabe
	//ASCII <-> Base64 = 3*8 Bit <-> 4*6 Bit -> wenn Base64 nicht vielfaches von 4 ist wurde falsch umgewandelt
	if (count % 4)
		return NULL;

	//Speicher für Output reservieren
	pos = out = malloc(count);
	if (out == NULL)
		return NULL;

	//durch Zeile 129 und 152 werden u.a. die Zeilenuzmbrüche herausgefiltert

	//umwandeln
	count = 0;
	for (int i = 0; i < len; i++) {
		tmp = dtable[src[i]];
		//nur gültige Zeichen bearbeiten
		if (tmp == 0x80)
			continue;

		//immer 4 gültige Werte zwischenspeichern
		in[count] = src[i];
		//immer 4 Indexe von den Zeichen in der Base64-Tabelle zwischenspeichern
		block[count] = tmp;
		count++;
		//wenn 4 Zeichen zwischengespeichert wurden, in ASCII umwandeln
		if (count == 4) {
			//erster 8 Bit Block = alle 6 Bits vom ersten 6 Bit Block und höchste zwei Bit vom zweiten 6 Bit Block
			*pos++ = (block[0] << 2) | (block[1] >> 4);
			//zweiter 8 Bit Block = niedrigste 4 Bit vom zweiten 6 Bit Block und höchste vier Bit vom dritten 6 Bit Block
			*pos++ = (block[1] << 4) | (block[2] >> 2);
			//dritter 8 Bit Block = niedrigste zwei Bit vom dritten 6 Bit Block und alle 6 Bit vom vierten 6 Bit Block
			*pos++ = (block[2] << 6) | block[3];
			count = 0;
		}
	}

	//wenn überhaupt Zeichen umgewandelt wurden
	if (pos > out) {
		//wenn das vorletzte Zeichen ein '=' ist, die Anzahl an decodierte Zeichen um 2 reduzieren (vorletzte und letzte wurden gepadded)
		if (in[2] == '=')
			pos -= 2;
		//wenn das letzte Zeichen ein '=' ist, die Anzahl an decodierte Zeichen um 1 reduzieren (letzte wurden gepadded)
		else if (in[3] == '=')
			pos--;
	}

	//Output-Länge berechnen durch Adresse von letztem decodierten - Adresse von erstem
	*out_len = pos - out;
	return out;
}