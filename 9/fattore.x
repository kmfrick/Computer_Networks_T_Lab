/* 
 * sala.x
 *	+definizione Input e struttura (2xstrutture semplici) della sala.
 * 	+definizione metodi e tipi richiesti/restituiti
 */

const NGIUDICI=4;

struct Input{
	string nome<255>;
	string operazione<12>;
};

struct Giudice{
	string nome<255> ;
};

struct Output {
	Giudice giudici [NGIUDICI];
};


program FATTORE {
	version FATTOREVERS{
		int ESPRIMI_VOTO(Input) = 1;
		Output CLASSIFICA_GIUDICI(void) = 2;
	} = 1;
} = 0x20000013;
