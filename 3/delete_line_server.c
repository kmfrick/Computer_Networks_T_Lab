/* Server che riceve un file e lo ridirige ordinato al client */

#include <dirent.h>
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

/********************************************************/
void handler(int signo) {
    int status;
    printf("S: esecuzione gestore di SIGCHLD\n");
    wait(&status);
}
/********************************************************/

int main(int argc, char **argv) {
    int listen_sd, conn_sd;
    int port, len, num;
    const int on = 1;
    struct sockaddr_in cliaddr, servaddr;
    struct hostent *host;

    /* CONTROLLO ARGOMENTI ---------------------------------- */
    if (argc != 2) {
        printf("DLS: Error: %s port\n", argv[0]);
        exit(1);
    } else {
        num = 0;
        while (argv[1][num] != '\0') {
            if ((argv[1][num] < '0') || (argv[1][num] > '9')) {
                printf("DLS: Secondo argomento non intero\n");
                exit(2);
            }
            num++;
        }
        port = atoi(argv[1]);
        if (port < 1024 || port > 65535) {
            printf("DLS: Error: %s port\n", argv[0]);
            printf("DLS: 1024 <= port <= 65535\n");
            exit(2);
        }
    }

    /* INIZIALIZZAZIONE INDIRIZZO SERVER ----------------------------------------- */
    memset((char *)&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);

    /* CREAZIONE E SETTAGGI SOCKET D'ASCOLTO --------------------------------------- */
    listen_sd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sd < 0) {
        perror("DLS: creazione socket ");
        exit(1);
    }
    printf("DLS: creata la socket d'ascolto per le richieste di ordinamento, fd=%d\n", listen_sd);

    if (setsockopt(listen_sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
        perror("DLS:set opzioni socket d'ascolto");
        exit(1);
    }
    printf("DLS: set opzioni socket d'ascolto ok\n");

    if (bind(listen_sd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("DLS: bind socket d'ascolto");
        exit(1);
    }
    printf("DLS: bind socket d'ascolto ok\n");

    if (listen(listen_sd, 5) < 0)  //creazione coda d'ascolto
    {
        perror("DLS:listen");
        exit(1);
    }
    printf("DLS: listen ok\n");

    // AGGANCIO GESTORE PER EVITARE FIGLI ZOMBIE
    signal(SIGCHLD, handler);

    /* CICLO DI RICEZIONE RICHIESTE --------------------------------------------- */
    while (1) {
        len = sizeof(cliaddr);
        if ((conn_sd = accept(listen_sd, (struct sockaddr *)&cliaddr, &len)) < 0) {
            //La accept puo' essere interrotta dai segnali inviati dai figli alla loro teminazione.
            if (errno == EINTR) {
                perror("DLS: Forzo la continuazione della accept");
                continue;
            } else
                exit(1);
        }

        if (fork() == 0) {  // figlio
            /*Chiusura FileDescr non utilizzati e ridirezione STDIN/STDOUT*/
            close(listen_sd);
            host = gethostbyaddr((char *)&cliaddr.sin_addr, sizeof(cliaddr.sin_addr), AF_INET);
            if (host == NULL) {
                printf("DLS: client host information not found\n");
                continue;
            }

            printf("DLS (figlio): host client e' %s \n", host->h_name);
            printf("DLS (figlio): Ricezione linea da eliminare: \n");
            int line = 0;
            int remaining = sizeof(int);
            int received = 0;
            char *line_buff = (char *)&line;
            int result = 0;

            while (remaining > 0) {
                result = recv(conn_sd, line_buff + received, remaining, 0);
                if (result > 0) {
                    remaining -= result;
                    received += result;
                } else if (result == 0) {
                    printf("Remote side closed his end of the connection before all data was received\n");
                    // probably a good idea to close socket
                    break;
                } else if (result < 0) {
                    printf("ERROR!\n");
                    // probably a good idea to close socket
                    break;
                }
            }
            printf("DLS: %d!\n", line);
            printf("DLS: Inizio lettura\n", line);
            int currentLine = 1;
            char buff;
            printf("DLS: ricevuto: ");
            printf("DLS: Riga corrente %d\n", currentLine);
            while (read(conn_sd, &buff, sizeof(buff)) > 0) {
                printf("%c", buff);
                if (currentLine != line) {
                    write(conn_sd, &buff, sizeof(buff));
                }
                if (buff == '\n') {
                    currentLine++;
                    printf("DLS: Riga corrente %d\n", currentLine);
                }
            }
            shutdown(conn_sd, SHUT_WR);

        }                // figlio
        close(conn_sd);  // padre chiude socket di connessione non di scolto
    }                    // ciclo for infinito
}
