#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

const size_t MAX_PATH_LENGTH = 256;
enum {
    SUCCESS,
    ERR_INVALID_ARGS,
    ERR_INVALID_PORT,
    ERR_UNAVAILABLE_PORT,
    ERR_SOCKET_CREAT,
    ERR_SOCKET_CONN
};

int main(int argc, char** argv) {
    char path[MAX_PATH_LENGTH];
    struct sockaddr_in servaddr;
    struct hostent* host;
    char c;
    int i;
    int port;
    int sd;
    
    // Controllo argomenti
    if (argc < 3) {
        printf("Usage: %s [HOST] [PORT]\n", argv[0]);
        exit(ERR_INVALID_ARGS);
    }
    i = 0;
    while ((c = argv[2][i++]) != '\0') {
        if (c < '0' || c > '9') {
            printf("LSC: Porta non valida\n");
            exit(ERR_INVALID_PORT);
        }
    }
    port = atoi(argv[2]);
    if (port < 1024 || port > 65535) {
        printf("LSC: Porta non disponibile\n");
        exit(ERR_UNAVAILABLE_PORT);
    }

    // Inizializzazione indirizzo server
    memset((char*)&servaddr, 0, sizeof(struct sockaddr_in));
    servaddr.sin_family = AF_INET;
    host = gethostbyname(argv[1]);
    servaddr.sin_addr.s_addr = ((struct in_addr*)(host->h_addr_list[0]))->s_addr;
    servaddr.sin_port = htons(port);

    // Ciclo principale
    printf("LSC: Inserire il nome del direttorio, EOF per terminare.\n");
    while (gets(path)) {
        // Creazione socket
        if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            printf("LSC: Impossibile creare una socket.\n");
            exit(ERR_SOCKET_CREAT);
        }
        if (connect(sd, (struct sockaddr*)&servaddr, sizeof(struct sockaddr)) < 0) {
            printf("LSC: Impossibile creare una connessione sulla socket %d\n", sd);
            exit(ERR_SOCKET_CONN);
        }	
        if (write(sd, path, strlen(path) + 1) <= strlen(path)) {
            printf("LSC: Errore in invio\n");
        };

        shutdown(sd, SHUT_WR);
        while (read(sd, &c, 1) > 0) {
            putchar(c);
        }
        close(sd);
        printf("LSC: Inserire il nome del direttorio, EOF per terminare.\n");
    }

    return SUCCESS;
}