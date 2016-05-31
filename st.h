typedef struct{
	int index;
	int type;
	int sl;
	int rangel;
	int rangeu;
	int isarr;
	} STentry;

typedef struct{
	STentry ST[100];
}scopest;

extern STentry ST[];
