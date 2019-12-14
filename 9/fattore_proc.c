/* sala_s.c
 * 	+implementazione delle procedure remote: "prenota" e "visualizza".
 *	+inizializzazione struttura.
 */

#include <fcntl.h>
#include <rpc/rpc.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "fattore.h"
/* STATO SERVER */
#define TAB_DIM 20
typedef struct {
    char nome[256];
    char giudice[256];
    char categoria;
    char nomefile[256];
    char fase;
    int voti;
} dati_candidato;

dati_candidato tabella[TAB_DIM];

typedef struct {
    char nome[256];
    int voti_tot;
} dati_giudice;

dati_giudice tabella_giudici[4];

static int inizializzato = 0;

void inizializza() {
    int i, j;

    if (inizializzato == 1) return;
	inizializzato = 1;
    // inizializzazione struttura dati
    for (i = 0; i < TAB_DIM; i++) {
        tabella[i].voti = 0;
        tabella[i].fase = 'L';
        tabella[i].categoria = 'L';
        strcpy(tabella[i].nome, "L");
        strcpy(tabella[i].giudice, "L");
        strcpy(tabella[i].nomefile, "L");
    }
    tabella[1].fase = 'B';
    tabella[1].categoria = 'U';
    strcpy(tabella[1].nome, "a");
    strcpy(tabella[1].giudice, "g1");
    strcpy(tabella[1].nomefile, "astjyx");
    tabella[2].fase = 'B';
    tabella[2].categoria = 'U';
    strcpy(tabella[2].nome, "b");
    strcpy(tabella[2].giudice, "g2");
    strcpy(tabella[2].nomefile, "artjyx");
    tabella[3].fase = 'B';
    tabella[3].categoria = 'D';
    strcpy(tabella[3].nome, "c");
    strcpy(tabella[3].giudice, "g2");
    strcpy(tabella[3].nomefile, "asgjyx");
    tabella[4].fase = 'D';
    tabella[4].categoria = 'D';
    strcpy(tabella[4].nome, "d");
    strcpy(tabella[4].giudice, "g1");
    strcpy(tabella[4].nomefile, "astjtx");
    tabella[5].fase = 'B';
    tabella[5].categoria = 'U';
    strcpy(tabella[5].nome, "e");
    strcpy(tabella[5].giudice, "g3");
    strcpy(tabella[5].nomefile, "astjyv");
    tabella[6].fase = 'B';
    tabella[6].categoria = 'U';
    strcpy(tabella[6].nome, "f");
    strcpy(tabella[6].giudice, "g2");
    strcpy(tabella[6].nomefile, "astjyx");
    tabella[7].fase = 'B';
    tabella[7].categoria = 'D';
    strcpy(tabella[7].nome, "g");
    strcpy(tabella[7].giudice, "g4");
    strcpy(tabella[7].nomefile, "astlyx");

    for (i = 0; i < NGIUDICI; i++) {
        tabella_giudici[i].voti_tot = 0;
        strcpy(tabella[i].giudice, "L");
    }
    strcpy(tabella_giudici[0].nome, "g1");
    strcpy(tabella_giudici[1].nome, "g2");
    strcpy(tabella_giudici[2].nome, "g3");
    strcpy(tabella_giudici[3].nome, "g4");
}
int ricerca(char *nome) {
    int i;
    for (i = 0; i < TAB_DIM; i++) {
        if (strcmp(nome, tabella[i].nome) == 0)
            return i;
    }
    return -1;
}

int totGiudice(char *gdc) {
    int i, tot = 0;
    for (i = 0; i < TAB_DIM; i++) {
        if (strcmp((tabella[i].giudice), gdc) == 0) {
            tot += tabella[i].voti;
        }
    }
    return tot;
}

int *esprimi_voto_1_svc(Input *input, struct svc_req *rqstp) {
    inizializza();
    // Cerco il nome passato in input nella tabella dei concorrenti. Se non presente restituisce codice di errore -1
	
    int i = ricerca(input->nome);
    if (i == -1) {
        printf("Nome non presente nella lista dei candidati.\n");
        static int err = -1;
        return &err;
    }
	
    if (strcmp((input->operazione), "aggiunta") == 0) {
        tabella[i].voti++;
    } else if (strcmp((input->operazione), "sottrazione") == 0) {
        if (tabella[i].voti == 0)
            printf("Operazione di sottrazione voto non possibile: numero voti nullo.\n");
        else
            tabella[i].voti--;
    } else {
        printf("Errore esprimi_voto_1: operazione non valida.\n");
    }
	
    return (&tabella[i].voti);
}

Output *classifica_giudici_1_svc(void *in, struct svc_req *rqstp) {
    inizializza();
    static Output res;
    int i, j;
    int maxi = 0;
    dati_giudice temp;
    for (i = 0; i < NGIUDICI; i++) {
        tabella_giudici[i].voti_tot = totGiudice(tabella_giudici[i].nome);
    }

    for (i = 0; i < NGIUDICI; i++) {  //selection "sort"
        for (j = 0; j < NGIUDICI; j++) {
            if (tabella_giudici[j].voti_tot > tabella_giudici[maxi].voti_tot)
                maxi = j;
        }
        res.giudici[i].nome = tabella_giudici[maxi].nome;  //copio il puntatore, xdr dovrebbe provvedere a passarlo per bene
        tabella_giudici[maxi].voti_tot = -1;
    }
    //algoritmo di ordinamento giudici

    return &res;
}  // visualizza
