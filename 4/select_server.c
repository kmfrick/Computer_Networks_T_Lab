#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define DIM_BUFF 512
#define DIM_FBUFF 256
#define max(x, y) (((x) > (y)) ? (x) : (y))

int main(int argc, char **argv) {
    DIR *dir1, *dir2;
    struct dirent *d1, *d2;
    int listenfd, connfd, udpfd, maxfdp1, fin, fout;
    int i;
	char is_separator[256];
    int pid, port, n, ris, len;
	int end_f, sep, equal;
    char buff[DIM_BUFF], str[DIM_BUFF], fBuff[DIM_FBUFF], *elim;
    const int on = 1;
    fd_set rset;
    struct sockaddr_in cliaddr, servaddr;
	// Inizializzazione  tabella dei separatori
	is_separator['\n']=1;
    memset(is_separator, 0, sizeof(is_separator));
    memset(is_separator, 1, (1 + '/') * sizeof(char));
    memset(is_separator + ':', 1, (1 + '@' - ':') * sizeof(char));
    memset(is_separator + '[', 1, (1 + '`' - '[') * sizeof(char));
    // Controllo argomenti
    if (argc != 2) {
        perror("errore numero argomenti");
        exit(13);
    } else {
        for (i = 0; argv[1][i] != '\0'; i++) {
            if (argv[1][i] < '0' || argv[1][i] > '9') {
                printf("Error: %s port\n", argv[0]);
                exit(13);
            }
        }
        port = atoi(argv[1]);
    }

    // Inizializzazione indirizzo server
    memset((char *)&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);  // no htonl
    servaddr.sin_port = htons(port);

    // NOTA: si possono usare lo stesso indirizzo e stesso numero di porta per le due socket
    // Creazione socket TCP di ascolto
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        perror("apertura socket TCP ");
        exit(1);
    }
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
        perror("set opzioni socket TCP");
        exit(2);
    }
    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind socket TCP");
        exit(3);
    }
    if (listen(listenfd, 5) < 0) {
        perror("listen");
        exit(4);
    }

    // Creazione socket UDP
    udpfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpfd < 0) {
        perror("apertura socket UDP");
        exit(5);
    }
    if (setsockopt(udpfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
        perror("set opzioni socket UDP");
        exit(6);
    }
    if (bind(udpfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind socket UDP");
        exit(7);
    }

    signal(SIGCHLD, SIG_IGN);            // Aggancio gestore
    FD_ZERO(&rset);                      // Pulizia e settaggio maschera file descriptor
    maxfdp1 = max(listenfd, udpfd) + 1;  // Prepara il primo parametro

    // Ciclo di ricezione eventi della select e preparazione maschera ad ogni giro
    while (1) {
        FD_SET(listenfd, &rset);
        FD_SET(udpfd, &rset);
        if (select(maxfdp1, &rset, NULL, NULL, NULL) < 0) {
            if (errno == EINTR)
                continue;
            else {
                perror("select");
                exit(8);
            }
        }

        // Gestione connessione TCP
        if (FD_ISSET(listenfd, &rset)) {
            if ((connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &len)) < 0) {
                perror("accept");
                exit(9);
            }

            pid = fork();
            if (pid < 0) {
                perror("fork");
                exit(40);
            }
            if (pid == 0) {
                while (read(connfd, str, DIM_BUFF) > 0) {
                    if (!(dir1 = opendir(str))) {
                        strcpy(str, "errore, directory non esistente\n");
                        write(connfd, str, strlen(str));
                    } else {
                        while ((d1 = readdir(dir1)) != NULL) {
                            if (strcmp(d1->d_name, ".") != 0 && strcmp(d1->d_name, "..") != 0 && (dir2 = opendir(strcat(strcat(strcpy(buff, str), "/"), d1->d_name)))) {
                                strcat(strcat(strcpy(buff, "Contenuto cartella ["), d1->d_name), "]:\n");
                                write(connfd, buff, strlen(buff));
                                while ((d2 = readdir(dir2)) != NULL) {
                                    strcat(strcat(strcpy(buff, " - ["), d2->d_name), "]\n");
                                    write(connfd, buff, strlen(buff));
                                }
                                closedir(dir2);
                            }
                        }
                        closedir(dir1);
                    }
                }

                // devo chiudere le socket ???
                exit(0);
            }
            close(connfd);
        }

        // Gestione connessione UDP
        if (FD_ISSET(udpfd, &rset)) {
            len = sizeof(struct sockaddr_in);
            if (recvfrom(udpfd, buff, DIM_BUFF, 0, (struct sockaddr *)&cliaddr, &len) < 0) {
                perror("recvfrom");
                continue;
            }
            //filename\0parola\0
            elim = buff + strlen(buff) + 1;
            printf("file %s elim %s\n", buff, elim);
            fin = open(buff, O_RDONLY);
            if (fin < 0) {
                ris = -1;
            } else {
                fout = open("out.txt", O_WRONLY | O_CREAT | O_TRUNC, 0777);
                if (fout < 0) {
                    perror("errore apertura file temp");
					exit(9);
                   }
                n=strlen(elim);
				ris=0;
				end_f=0;
				while(!end_f){
					equal=1;
					sep=0;
					//leggo la parola e confronto
					for(i=0;!sep && i<DIM_FBUFF&& (read(fin, fBuff+i, 1)>0);  i++){
						if(is_separator[fBuff[i]])
							sep=1;
						else if(elim[i]!=fBuff[i])
							equal=0;
					}
					if(equal && (i-sep)==n){
						ris++;
						if(sep)
							write(fout, fBuff+i-1, 1);//stampo il terminatore
					}
					else
						write(fout, fBuff, i);//stampo la parola
					if(!sep)
						end_f=1;
				}		
			}
            close(fin);
            close(fout);
            rename("out.txt", buff);
            if (sendto(udpfd, &ris, sizeof(int), 0, (struct sockaddr *)&cliaddr, len) < 0) {
                perror("sendto");
                continue;
            }     
           
        }
    }

    return 0;
}
