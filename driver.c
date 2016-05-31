#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#include "y.tab.h"
#include "check.h"
#include "generate.h"
extern FILE * yyin;
extern tree root;
FILE * outfile;
int main(int argc, char * argv[]){
	if((yyin = fopen(argv[1], "r")) == 0) {printf("error");}
	if (argc != 3) {
		fprintf (stderr, "%s: Insufficient Arguments\n", argv[0]);
		exit(1);
	}

	yyparse();
	//printTree(root);
	check(root);
	if ((outfile = fopen (argv[2], "w")) == 0L) {
		fprintf (stderr, "%s: Can't open Outfile File %s\n", argv[0], argv[2]);
		exit(1);
	}

	gen_program(root);
	fclose(yyin);
	fclose(outfile);
	return 0;
}
