struct FR {
	long filep;
	int  LChere;
};

void code (int opcode), code1 (int opcode, int arg);
struct FR codeFR (int opcode);
void fixFR (struct FR here, int lc);
void prLC(void);
void prNL (void);

extern FILE *outfile;
extern int	LC;
