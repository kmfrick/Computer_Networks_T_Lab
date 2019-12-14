/* operazioni_client.c
 *
 */

#include <stdio.h>
#include <rpc/rpc.h>
#include <stdlib.h>
#include "scan.h"

char check_num(char* c) {
	int i;
	for (i = 0; i < strlen(c); i++) {
		if (c[i] < '0' || c[i] > '9') return 0;
	} 
	return 1;
}

int main(int argc, char *argv[]){

	CLIENT *cl;
	char *server;
	Result *resFile;
	int *resDir;
	Input input;
	char soglia[256];
	char op[2];

	// Controllo argomenti
	if (argc != 2) {
		fprintf(stderr, "Client usage: %s host\n", argv[0]);
		exit(1);
	}

	server = argv[1];

	// Connessione
	cl = clnt_create(server, SCANPROG, SCANVERS, "udp");
	if (cl == NULL) {
		clnt_pcreateerror(server);
		exit(1);
	}

	// Ciclo di richieste: ad ogni iterazione richiede il tipo di operazione da eseguire
	// e lo salva nella stringa op, dimensionata opportunamente in modo da contenere carattere D o F
	// e il terminatore di stringa.
	printf("Premi F per file scan, D per dir scan o EOF per terminare: ");
	input.dir = malloc(256 * sizeof(char));
	while (gets(op))
	{
		
		if(op[0] == 'F'){ // Op F: richiedo uno scan sul file per estrarre numero di caratteri, parole e righe.
			printf("Inserire il nome del file: ");
			gets(input.dir);	//salvo il nome del file direttamente nella struttura dati
			resFile = filescan_1(&input.dir, cl);	// invoco il metodo prodotto dal file XDR
			if (resFile == NULL) {
				printf("Errore nell'analisi dei file.\n");
				printf("Premi F per file scan, D per dir scan o EOF per terminare: ");
				continue;
			}
			if (resFile->caratteri < 0) {
				printf("Errore nell'analisi dei file.\n");
				printf("Premi F per file scan, D per dir scan o EOF per terminare: ");
				continue;
			}
			// Stampo il risultato
			printf("Caratteri: %d\n", resFile->caratteri);
			printf("Parole: %d\n", resFile->parole);
			printf("Righe: %d\n", resFile->righe);
		}
		else if(op[0] == 'D'){ // Op D: richiedo uno scan su una directory per ottenere 
							   // il numero di file contenuti che superano il valore di soglia
			printf("Inserire il nome del direttorio: ");
			gets(input.dir);
			printf("Inserire una soglia numerica: ");
			gets(soglia);
			if (!check_num(soglia)) {
				printf("Soglia non valida\n");
				printf("Premi F per file scan, D per dir scan o EOF per terminare: ");
				continue;
			}
			input.max_size = atoi(soglia);
			resDir = dirscan_1(&input, cl); // invoco il metodo prodotto dal file XDR
			if (resDir == NULL) {
				printf("Errore nella ricerca dei file che superano la soglia.\n");
				printf("Premi F per file scan, D per dir scan o EOF per terminare: ");
				continue;
			}
			if(*resDir > -1)	// se il risultato prodotto dallo scan è -1, c'è stato un errore.
				printf("Numero di file la cui dimensione supera la soglia inserita: %d\n", *resDir);
			else
				// Altrimenti, se il risultato è positivo (o nullo), viene stampato.
				printf("Errore nella ricerca dei file che superano la soglia.\n");
		}
		else{
			// Gli unici due comandi possibili sono F, D o EOF.
			printf("Errore: comando non valido.\n");
		}
		printf("Premi F per file scan, D per dir scan o EOF per terminare: ");
	} // while gets(op)

	// Libero le risorse
	clnt_destroy(cl);
	free(input.dir);
	printf("Termino...\n");
	exit(0);
}
