#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_STRING_LENGTH 256

int main(int argc, char* argv[]) {
    int fd;                       // File da aprire in scrittura
    int written;                  // Numero di caratteri scritti (ritorno della write())
    char* file_out;               // Nome file di output
    char buf[MAX_STRING_LENGTH];  // Buffer scrittura
    int i = 0;

    // Controllo argomenti
    if (argc != 2) {
        perror("Numero di argomenti sbagliato. ");
        exit(EXIT_FAILURE);
    }
    file_out = argv[1];  // Utilizzato spesso, mai modificato -> salvato in una variabile
    // Apertura file
    fd = open(file_out, O_WRONLY | O_CREAT | O_TRUNC, 00640);
    if (fd < 0) {
        perror("P0: Impossibile creare/aprire il file");
        exit(EXIT_FAILURE);
    }

    printf("Inserire testo: ");
    while ((buf[i] = getchar()) != EOF) {  // Caratteri disponibili
        i++;
        if (i == MAX_STRING_LENGTH) {     // Buffer pieno
            written = write(fd, buf, i);  // Scrivi su file, svuota buffer e riprendi
                                          // i invece di strlen per evitare vulnerabilità di tipo buffer overflow
            if (written <= 0) {
                perror("P0: errore nella scrittura sul file");
                exit(2);
            }
            i = 0;
        }
    }
    buf[i] = '\0';  // Terminazione stringa

    written = write(fd, buf, strlen(buf));  // Stampa su file il buffer

    if (written <= 0) {
        perror("P0: errore nella scrittura sul file");
        exit(2);
    }
    /*
	while ((temp = getchar()) != EOF) {
		written = write(fd, &temp, 1);
		if (written <= 0) {
			perror("errore nella scrittura sul file");
			exit(2);
		}
	}
	*/

    close(fd);
}
