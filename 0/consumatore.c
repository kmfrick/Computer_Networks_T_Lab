#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int lettera_presente(char c, char* stringa);

int main(int argc, char* argv[]) {
    char* file_in;  // Nome del file input
    int fd;         // File input o stdin
    int i;
    int filterlen;  // Lunghezza della stringa filtro
    char c;

    // Controllo argomenti
    if (argc != 2 && argc != 3) {
        perror("Numero di argomenti sbagliato");
        exit(1);
    }

    if (argc == 2) {  // Legge da stdin se non viene dato un argomento
        fd = STDIN_FILENO;
    }

    if (argc == 3) {  // Apertura file
        file_in = argv[2];
        fd = open(file_in, O_RDONLY);
        if (fd < 0) {
            perror("P0: Impossibile aprire il file");
            exit(2);
        }
    }
    filterlen = strlen(argv[1]);

    while (read(fd, &c, sizeof(char)) > 0) {
        if (lettera_presente(c, argv[1]) == 0)  // Se il carattere non è
                                                // nella stringa filtro, lo stampa
            putchar(c);
    }
    putchar('\n');

    close(fd);
}

// Restituisce 1 se presente, 0 se non presente
int lettera_presente(char c, char* stringa) {
    int i;
    for (i = 0; i < strlen(stringa); i++) {
        if (stringa[i] == c)
            return 1;
    }
    return 0;
}
