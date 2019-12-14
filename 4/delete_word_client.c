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

#define MAX_WORD_LENGHT 256

int main(int argc, char **argv) {
    char nome_file[FILENAME_MAX + 1];
    char parola[MAX_WORD_LENGHT];
    char buff[FILENAME_MAX + MAX_WORD_LENGHT + 1];
    int sd, port, num1, len, ris;
    struct hostent *host;
    struct sockaddr_in clientaddr, servaddr;

    // Controllo argomenti
    if (argc != 3) {
        printf("DWC: %s serverAddress serverPort\n", argv[0]);
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
            printf("DWC: serverPort non intero.\n");
            exit(2);
        }
        num1++;
    }
    port = atoi(argv[2]);
    if (port < 1024 || port > 65535) {
        printf("DWC: serverPort non valida.\n");
        exit(3);
    }
    if (host == NULL) {
        printf("DWC: host non trovato.\n");
        exit(4);
    } else {
        servaddr.sin_addr.s_addr = ((struct in_addr *)(host->h_addr))->s_addr;
        servaddr.sin_port = htons(port);
    }

    // Creazione socket
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd < 0) {
        printf("DWC: apertura socket non riuscita.\n");
        exit(5);
    }

    // Binding socket
    if (bind(sd, (struct sockaddr *)&clientaddr, sizeof(clientaddr)) < 0) {
        perror("DWC: bind socket non riuscita.\n");
        exit(6);
    }

    printf("DWC: Nome del file remoto, EOF per terminare: ");
    while (gets(buff)) {
        printf("DWC: File da aprire: %s\n", buff);

        int lenfilename = strlen(buff);
        // Lettura e invio parola da eliminare
        printf("DWC: inserire parola da eliminare: ");
        gets(buff + lenfilename + 1);

        // Invio nome file e parola da eliminare
        len = sizeof(servaddr);
        if (sendto(sd, buff, sizeof(char) * (strlen(buff) + strlen(buff + lenfilename + 1) + 2), 0, (struct sockaddr *)&servaddr, len) < 0) {
            perror("DWC: sendto nome file non riuscita.\n");
            continue;
        }

        printf("DWC: Attesa del risultato...\n");
        if (recvfrom(sd, &ris, sizeof(int), 0, (struct sockaddr *)&servaddr, &len) < 0) {
            perror("DWC: recvfrom non riuscita.\n");
            continue;
        }

        if (ris > 0) {
            printf("DWC: Esito dell'operazione: %d\n", ris);
        } else {
            printf("DWC: il file remoto non esiste.\n");
        }

        printf("DWC: Nome del file remoto, EOF per terminare: ");
    }

    close(sd);

    return 0;
}