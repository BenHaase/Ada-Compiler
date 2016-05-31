#include <stdio.h>
#include "tree.h"
#include "y.tab.h"
#include "instr.h"
#include "code.h"

#define addr_of(t) (2* (t->value -2))
typedef struct{
	int val;
	int lb;
	int ub;
}savebounds;
savebounds ab[10];
int at = 0;

static void gen_expr (tree t)
{
	int	n;
	if (t == NULL) fprintf (stderr, "Internal error 11\n");
	switch (n = t->kind) {
		case Or: case And:
		case Xor: case Not:{
			gen_expr(t->first);
			gen_expr(t->second);
		}
			break;
		case Eq : case NEq :
		case Lt : case LtEq :
		case Gt : case GtEq :
			gen_expr (t->first); gen_expr (t->second);
			if (n == Gt || n == LtEq) code (SWAPW);
			code (SUBI);
			code ( (n == Eq || n == NEq) ? TSTEQI : TSTLTI);
			if (n == NEq || n == LtEq || n == GtEq) code (NOTW);
			break;
		case Plus : case Minus :
		case Star : case Slash :
			gen_expr (t->first); gen_expr (t->second);
			code (n == Plus ? ADDI : n == Minus ? SUBI : n == Star ? MULI : DIVI);
			break;
		case Ident : 
			code1 (PUSHW, addr_of (t)); code (GETSW);
			break;
		case IntConst :
			code1 (PUSHW, t->value);
			break;
		case Dd: {
			gen_expr(t->first);
			code(PUTSW);
			prNL();
			prLC();
			gen_expr(t->second);
			code(DUPW);
		}
			break;
		case LBrack : {
			int i, a;
			for(i = 0; i < 10; i++){
				if(t->first->value == ab[i].val){
					a = i;
				}
			}
			code1(PUSHW, addr_of(t->first));
			gen_expr(t->second);
			code(DUPW);
			code1(PUSHW, ab[a].lb);
			code(SUBI);
			code(TSTLTI);
			code1(RGOZ, 1);
			code(HALT);
			code(DUPW);
			code1(PUSHW, ab[a].ub);
			code(SWAPW);
			code(SUBI);
			code(TSTLTI);
			code1(RGOZ, 1);
			code(HALT);
			code1(PUSHW, addr_of(t->second));
			code(MULI);
			code(ADDI);
			code(GETSW);
		}
			break;
		case True : case False :
			code1(PUSHW, addr_of(t));
			break;
		default:
			fprintf (stderr, "Internal error 12 %i\n", n);
	}
}

static void generate (tree t)
{
	for (t; t != NULL; t = t->next){ 
		switch (t->kind) {
			case Integer : case Boolean :
				/* do nothing */
				break;
			case Assign : {
				prLC ();
				code1 (PUSHW, addr_of (t->first));//unsure 
				gen_expr (t->second);//unsure was t->first
				code (PUTSW);
				prNL ();
			}
				break;
			case If : {
				struct FR	fix1, fix2;
				gen_expr (t->first);
				fix1 = codeFR (RGOZ);
				prNL();
				generate (t->second);
				if (t->third != NULL) {
					prLC();
					fix2 = codeFR (RGOTO);
					prNL();
					if(t->third->kind != Else) prLC();
					fixFR (fix1, LC);
					generate (t->third);
					fixFR (fix2, LC);
					}
				else
					fixFR (fix1, LC);
			}
				break;
			case Else : {
				generate(t->first);
			}
				break;
			case For : {
				struct FR fix;
				int top = LC;
				prLC();
				//gen_expr(t->first);
				code1(PUSHW, addr_of(t->first));
				gen_expr(t->second);
				prNL();
				prLC();
				code1(PUSHW, addr_of(t->first));
				code(GETSW);
				code(SUBI);
				code(TSTEQI);
				fix = codeFR(RGOZ);
				prNL();
				prLC();
				generate(t->third);
				fixFR(fix, LC);
				prLC();
				code1(PUSHW, addr_of(t->first));
				code(DUPW);
				code(GETSW);
				code1(PUSHW, 1);
				code(ADDI);
				code(PUTSW);
				prNL();
				prLC();
				code(DUPW);
				code1(RGOTO, 266);
				prNL();
				prLC();
				code1(PUSHW, -1);
				code(CHSPS);
				prNL();
			}
				break;
			case Exit : {
				struct FR	fix;
				prLC ();
				gen_expr (t->first);
				fix = codeFR (RGOZ);
				prNL ();
				generate (t->second);
				fixFR (fix, LC);
			}
				break;
			/*case Exit : {
				struct FR fix;
				prLC();
				if(t != NULL) gen_expr(t->first);
				fix = codeFR(RGOZ);
				fixFR(fix, LC);
				prNL();
			}
				break;*/
			case Colon :
				break;
			default :
				fprintf (stderr, "Internal error 10, node %s [%d]\n", id_name(t->first->value), t->kind);//was TokName[t->kind]
			}
	}
}

void arrbounds(tree t)
{
        for (t; t!= NULL; t = t->next) {
                int     type = t->second->kind;
                tree    p;
                for (p = t->first; p != NULL; p = p->next) {
                        int     pos = p->value;
                        if(type == Array){
                                ab[at].val = pos;
                                ab[at].lb = t->second->first->first->value;
                                ab[at].ub = t->second->first->second->value;
                                at++;
                        }
                }
        }
}

void gen_program (tree t)
{
	int k;
	for(k = 0; k < 10; k++){
		ab[k].val = -1;
		ab[k].lb = 0;
		ab[k].ub = 0;
	}
	pr_directive (".CODE .ENTRY 0");
	arrbounds(t->second);
	generate(t->second);
	generate(t->third);
	prLC ();
	code (HALT);
	prNL ();
}

