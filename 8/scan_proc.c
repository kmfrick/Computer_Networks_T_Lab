/* scan_proc.c
 * 	+implementazione della procedura remota "scan_file" e "scan_dir".
 * 	+include scan.h.
 */

#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <rpc/rpc.h>
#include <unistd.h>

#include "scan.h"

#define SZ 256

Result *filescan_1_svc(char **filename, struct svc_req *req) {
    static Result result;
    static Result ERR;
    ERR.caratteri = -1;
    ERR.parole = -1;
    ERR.righe = -1;
    int fd;
    result.parole = 0;
    result.caratteri = 0;
    result.righe = 0;
    char word_started = 0;
    char c;
    fd = open(*filename, O_RDONLY);
    if (fd < 0) return &ERR;

    while (read(fd, &c, 1) > 0) {
        result.caratteri++;
        if (isspace(c)) {
            result.parole += word_started;
            if (c == '\n') {
                result.righe++;
            }
            word_started = 0;
        } else {
            word_started = 1;
        }
    }
    close(fd);
    return &result;
}

int *dirscan_1_svc(Input *input, struct svc_req *rp) {
    /*
	* Il parametro di uscita è statico ed è da allocare.
	* Non serve allocare *msg, fatto dal supporto rpc.
	*/
    static int ERR = -1;
    static int size;

    
    long max = input->max_size;
    
    // Parte dall esercitazione 4 per cercare nelle sotto-directory fatto da Rick
    // Da adattare per cercare nella sola directory data per file di dim maggiore( o minore non ricordo) come da richiesta

    int file_count = 0;
    /* Per contare i file */
    DIR *dirp;
    struct dirent *entry;

    /* Per controllare la loro grandezza */
    struct stat st;

    if ((dirp = opendir(input->dir)) == NULL) {
        perror("S");
        return &ERR;
    }
    if (chdir(input->dir) < 0) {
        perror("S");
        return &ERR;
    }
    while ((entry = readdir(dirp)) != NULL) {
    
        if (entry->d_type == DT_REG) {  // se il file e' veramente un file prosegue
            stat(entry->d_name, &st);
    
            if (st.st_size > max)  // grandezza file > maggiore della grandezza data
                file_count++;
        } else {
    
        }
    }
    if (chdir("..") < 0) {
        perror("S");
        return &ERR;
    }
    closedir(dirp);
    size = file_count;
    return (&size);
}
