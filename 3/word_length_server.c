//parallelo
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define MAXL 255

int main(int argc, char **argv) {
    char is_separator[256];
    char buf[MAXL + 1];
    int sd, port, len, ris, pid, fd, counter, i, nReceived;
    unsigned char ch;
    const int on = 1;
    struct sockaddr_in cliaddr, servaddr;
    struct hostent *clienthost;

    // Inizializzazione  tabella dei separatori
    memset(is_separator, 0, sizeof(is_separator));
    memset(is_separator, 1, (1 + '/') * sizeof(char));
    memset(is_separator + ':', 1, (1 + '@' - ':') * sizeof(char));
    memset(is_separator + '[', 1, (1 + '`' - '[') * sizeof(char));

    // Controllo argomenti
    if (argc != 2) {
        printf("Errore: %s port\n", argv[0]);
        exit(2);
    } else {
        // Verifica intero
        for (i = 0; argv[1][i] != '\0'; i++)
            if (argv[1][i] < '0' || argv[1][i] > '9') {
                printf("Errore: %s port\n", argv[0]);
                exit(2);
            }
        port = atoi(argv[1]);
        if (port < 1024 || port > 65535) {  // Porta nel range porte disponibili
            printf("Errore: %s port\n", argv[0]);
            exit(2);
        }
    }
    memset((char *)&servaddr, 0, sizeof(servaddr));  // Inizializzazione indirizzo
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);
    sd = socket(AF_INET, SOCK_DGRAM, 0);  // Creazione, bind e settaggio socket
    if (sd < 0) {
        perror("Errore: creazione socket non riuscita.\n");
        exit(1);
    }
    printf("Server: creata la socket, sd = %d\n", sd);
    if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
        perror("Errore: set opzioni socket non riuscita.\n");
        exit(1);
    }
    if (bind(sd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Errore: bind socket non riuscita.\n");
        exit(1);
    }
    printf("Server: bind socket ok.\n");
    signal(SIGCHLD, SIG_IGN);
    // Ciclo infinito di ricezione e servizio
    while (1) {
        len = sizeof(struct sockaddr_in);
        if ((nReceived = recvfrom(sd, buf, MAXL, 0, (struct sockaddr *)&cliaddr, &len)) < 0) {
            perror("Errore: recvfrom non riuscita.\n");
            continue;
        }
        if (!(pid = fork())) {      //figlio
            buf[nReceived] = '\0';  //metto il terminatore
            fd = open(buf, O_RDONLY);
            if (fd < 0)
                ris = -1;
            else {
                ris = 0;
                counter = 0;
                while (read(fd, &ch, sizeof(char))) {  //calcolo parola più lunga
                    if (is_separator[ch]) {
                        if (counter > ris) ris = counter;
                        counter = 0;
                    } else
                        counter++;
                }
                close(fd);
            }
            if (sendto(sd, &ris, sizeof(int), 0, (struct sockaddr *)&cliaddr, len) < 0) {
                perror("Errore: sendto non riuscita.\n");
                exit(1);
            }
            return 0;
        }
        if (pid < 0) {
            perror("Errore: fork non riuscita.\n");
        }
    }
}