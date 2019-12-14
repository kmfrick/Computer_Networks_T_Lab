#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <rpc/rpc.h>
#include "fattore.h"
#include <string.h>

int main(int argc, char *argv[]){
	char *server, op[2];
	int i;
	int ris;
	CLIENT *cl;
	Input input;
	Output *output;

	// Controllo argomenti
	if (argc != 2) {
		fprintf(stderr, "Client usage: %s host\n", argv[0]);
		exit(1);
	}
	server = argv[1];

	// Connessione
	cl = clnt_create(server, FATTORE, FATTOREVERS, "udp");
	if (cl == NULL) {
		clnt_pcreateerror(server);
		exit(1);
	}

	input.nome = malloc(255 * sizeof(char));
	input.operazione = malloc(12 * sizeof(char));

	printf("Premi 1 per Stampa classifica giudici, 2 per aggiungere un voto a un candidato, 3 per sottrarre un voto a un candidato o EOF per terminare: ");
	while (gets(op)) {
		if(op[0] == '1'){ 
			output = classifica_giudici_1(&input, cl);
			i = 0;
			while (i < NGIUDICI) {
				printf("%d - %s\n", i+1, output->giudici[i]);
				i++;
			}
		}
		else if(op[0] == '2') {
			printf("Inserire il nome del candidato: ");
			gets(input.nome);
			strcpy(input.operazione, "aggiunta");
			ris = *esprimi_voto_1(&input, cl);
			if (ris < 0) {
				printf("Errore nell'espressione del voto.\n");
			} else {
				printf("I voti attuali di %s sono: %d\n", input.nome, ris);
			}
			
		} else if(op[0] == '3') {
			printf("Inserire il nome del candidato: ");
			gets(input.nome);
			strcpy(input.operazione, "sottrazione");
			ris = *esprimi_voto_1(&input, cl);
			if (ris < 0) {
				printf("Errore nell'espressione del voto.\n");
			} else {
				printf("I voti attuali di %s sono: %d\n", input.nome, ris);
			}
		}
	    printf("Premi 1 per Stampa classifica giudici, 2 per aggiungere un voto a un candidato, 3 per sottrarre un voto a un candidato o EOF per terminare: ");
	}

	clnt_destroy(cl);
	free(input.nome);
	free(input.operazione);
	exit(0);
}