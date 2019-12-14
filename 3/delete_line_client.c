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

#define DIM_BUFF 256

int main(int argc, char **argv) {
    char fileName[FILENAME_MAX + 1], fileRow[DIM_BUFF];
    int port, isNum, rowNumber, sd, ifd, ofd, procedi, fileRowNumber, nread;
    struct sockaddr_in servaddr;
    struct hostent *host;

    if (argc != 3) {
        printf("Usage: %s serveraddress serverport\n", argv[0]);
        exit(1);
    }

    isNum = 0;
    while (argv[2][isNum] != '\0') {
        if ((argv[2][isNum] < '0') || (argv[2][isNum] > '9')) {
            printf("DLC: porta non valida\n");
            exit(2);
        }
        isNum++;
    }

    port = atoi(argv[2]);
    if (port < 1024 || port > 65535) {
        printf("DLC: porta non valida\n");
        exit(2);
    }

    host = gethostbyname(argv[1]);

    memset((char *)&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = ((struct in_addr *)(host->h_addr_list[0]))->s_addr;
    servaddr.sin_port = htons(port);

    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
        perror("DLC: errore apertura socket");
        exit(3);
    }

    if (connect(sd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr)) < 0) {
        perror("DLC: errore di connessione");
        exit(4);
    }

    procedi = 1;
    while (procedi) {
        printf("DLC: inserisci il nome del file di testo a cui vuoi cancellare una riga\nDLC: ");
        gets(fileName);

        procedi = 0;
        if ((ifd = open(fileName, O_RDONLY)) < 0) {
            printf("DLC: impossibile aprire il file selezionato\n");
            procedi = 1;
        }
    }

    ofd = open("tmp.txt", O_WRONLY | O_CREAT | O_TRUNC, 0777);
    if (ofd < 0) {
        printf("DLC: impossibile creare file finale\n");
        exit(3);
    }

    procedi = 1;
    while (procedi) {
        rowNumber = -1;
        printf("DLC: inserisci numero di riga contenuta nel file da eliminare\nDLC: ");
        scanf("%d", &rowNumber);

        procedi = 0;
        if (rowNumber < 1) {
            printf("DLC: numero non valido\n");
            procedi = 1;
        }
    }

    send(sd, &rowNumber, sizeof(int), 0);
    int pid;
    if ((pid = fork()) < 0) {
        printf("DLC: error on fork\n");
    } else if (pid == 0) {
        while ((nread = read(ifd, fileRow, DIM_BUFF)) > 0) {
            write(sd, fileRow, nread);
        }
        shutdown(sd, 1);
    } else if (pid > 0) {
        while ((nread = read(sd, fileRow, DIM_BUFF)) > 0) {
            write(ofd, fileRow, nread);
            write(1, fileRow, nread);
        }
        shutdown(sd, 0);
    }

    close(ifd);
    close(ofd);
    close(sd);

    exit(0);
}
