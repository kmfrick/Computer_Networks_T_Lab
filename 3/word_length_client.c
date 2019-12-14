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

int main(int argc, char **argv) {
    char nome_file[FILENAME_MAX + 1];
    int sd, port, num1, len, ris;
    struct hostent *host;
    struct sockaddr_in clientaddr, servaddr;

    // Controllo argomenti
    if (argc != 3) {
        printf("Errore: %s serverAddress serverPort\n", argv[0]);
        exit(1);
    }

    // Inizializzazione indirizzo client e server e fine controllo argomenti
    memset((char *)&clientaddr, 0, sizeof(struct sockaddr_in));
    clientaddr.sin_family = AF_INET;
    clientaddr.sin_addr.s_addr = INADDR_ANY;
    clientaddr.sin_port = 0;
    memset((char *)&servaddr, 0, sizeof(struct sockaddr_in));
    servaddr.sin_family = AF_INET;
    host = gethostbyname(argv[1]);

    // Verifica correttezza porta e host:
    num1 = 0;
    while (argv[2][num1] != '\0') {
        if ((argv[2][num1] < '0') || (argv[2][num1] > '9')) {
            printf("Errore: serverPort non intero.\n");
            exit(2);
        }
        num1++;
    }
    port = atoi(argv[2]);
    if (port < 1024 || port > 65535) {
        printf("Errore: serverPort non valida.\n");
        exit(3);
    }
    if (host == NULL) {
        printf("Errore: host non trovato.\n");
        exit(4);
    } else {
        servaddr.sin_addr.s_addr = ((struct in_addr *)(host->h_addr))->s_addr;
        servaddr.sin_port = htons(port);
    }

    // Creazione socket
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd < 0) {
        printf("Errore: apertura socket non riuscita.\n");
        exit(5);
    }

    // Binding socket
    if (bind(sd, (struct sockaddr *)&clientaddr, sizeof(clientaddr)) < 0) {
        perror("Errore: bind socket non riuscita.\n");
        exit(6);
    }

    printf("Nome del file remoto, EOF per terminare: ");
    while (gets(nome_file)) {
        printf("File da aprire: %s\n", nome_file);

        // Richiesta operazione
        len = sizeof(servaddr);
        if (sendto(sd, nome_file, sizeof(char) * (strlen(nome_file) + 1), 0, (struct sockaddr *)&servaddr, len) < 0) {
            perror("Errore: sendto non riuscita.\n");
            continue;
        }

        printf("Attesa del risultato...\n");
        if (recvfrom(sd, &ris, sizeof(int), 0, (struct sockaddr *)&servaddr, &len) < 0) {
            perror("Errore: recvfrom non riuscita.\n");
            continue;
        }

        if (ris > 0) {
            printf("Esito dell'operazione: %d\n", ris);
        } else {
            printf("Errore: il file remoto non esiste.\n");
        }

        printf("Nome del file remoto, EOF per terminare: ");
    }

    close(sd);

    return 0;
}