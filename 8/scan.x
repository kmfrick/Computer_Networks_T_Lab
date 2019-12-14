/* 
 * scan.x
 *	+definizione struttura Operandi e Metodi.
 * 	+rpcgen scan.x --> genera i seguenti file
 *		- scan.h --> libreria da includere in scan_proc.c
 *					e scan_client.c.
 *		- scan_xdr.c --> routine di conversione.
 * 		- scan_clnt.c; scan_svc.c --> stub C e S.
 */

struct Result{
	int caratteri;
	int parole;
	int righe;
};

struct Input{
	string dir<255>;
	long max_size;
};

program SCANPROG {
	version SCANVERS {
		Result FILESCAN(string) = 1;
		int DIRSCAN(Input) = 2;
	} = 1;
} = 0x20000013;


