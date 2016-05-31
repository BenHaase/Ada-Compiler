#include <stdio.h>
#include "tree.h"
#include "y.tab.h"
#include "st.h"
#include "check.h"

extern int top;
int sl = 0;
scopest s[15];

void prST (void)
{
        int i;
        printf ("SYMBOL TABLE  SL:%i\n", sl);
        for (i = 0; i <= top; i++) {
		if(s[sl].ST[i].sl < (sl + 1)){
                	int     t = s[sl].ST[i].type;
			int a = s[sl].ST[i].isarr;
			printf ("%3d %-10s\t%s%s  SL: %i  ", i, id_name (i),
			t == Integer ? "Integer" : t == Boolean ? "Boolean" : "<none> ", a == Array ? "[]" : "  ", s[sl].ST[i].sl);
			if(s[sl].ST[i].isarr == Array){
				printf(" rangeL:%i  rangeU:%i", s[sl].ST[i].rangel, s[sl].ST[i].rangeu);
			}
			printf("\n");
		}
	}
}

static int check_expr (tree t)
{
        if (t == NULL) {
                fprintf (stderr, "You shouldn't be here; missing expression\n");
                return NoType;
        }
        switch (t->kind) {
		case Or: case And:
		case Xor: case Not:{
			int t1 = check_expr(t->first), t2 = check_expr(t->second);
			if(t2 != t2){
				fprintf(stderr, "Type mismatch in Boolean comparison\n");
				return (t->value = NoType);
			}
			else
				return (t->value = Boolean);
		}
		break;

                case Eq: case NEq:
                case Lt: case LtEq:
                case Gt: case GtEq: {
                        int t1 = check_expr (t->first), t2 = check_expr (t->second);
                        if (t1 != t2) {
                                fprintf (stderr, "Type mismatch in comparison\n");
                                return (t->value = NoType);
                        }
                        else
                                return (t->value = Boolean);
		}
                break;

                case Plus : case Minus :
                case Star : case Slash : {
                        int t1 = check_expr (t->first), t2 = check_expr (t->second);
                        if (t1 != Integer || t2 != Integer) {
                                fprintf (stderr, "Type mismatch in binary arithmetic expression\n");
                                return (t->value = NoType);
			}
                        else
                                return (t->value = Integer);
		}
                break;

                case Ident: {
                        int pos = t->value;
                        if (s[sl].ST[pos].index == 0) {
                                s[sl].ST[pos].index = pos;
                                s[sl].ST[pos].type = Integer;
				s[sl].ST[pos].sl = sl;
                        }
                        return s[sl].ST[pos].type;
                }
                break;

		case Dd: {
			int t1 = check_expr(t->first);
			int t2;
			if(t->second != NULL) t2 = check_expr(t->second);
		}
		break;

		case LBrack: {
			int pos = t->first->value;
			if((s[sl].ST[pos].isarr == Array) && (check_expr(t->first) == check_expr(t->second))){
				return Integer;
			}
			else{
				fprintf(stderr, "Error in array reference");
			}
		}
		break;

                case IntConst:
                        return Integer;         /* t->value contains other info */
		break;

		case True: case False: {
			return Boolean;
		}
		break;

                default:
                        fprintf (stderr, "You shouldn't be here; invalid expression operator %i\n", t->kind);
        }
}

static void handle_decls (tree t)
{
        for (t; t!= NULL; t = t->next) {
                int     type = t->second->kind;
                tree    p;
                if ((type != Integer) && (type != Boolean) && (type != Array)) {
                        fprintf (stderr, "Bad type in decl %i \n", type); return;
                }
                for (p = t->first; p != NULL; p = p->next) {
                        int     pos = p->value;
			if(s[sl].ST[pos].index != 0){
				if(s[sl].ST[pos].sl == sl){
					fprintf(stderr, "Error redefinition of var in scope\n");
					return;
				}
			}
                        s[sl].ST[pos].index = pos;
                        s[sl].ST[pos].type = type;
			s[sl].ST[pos].sl = sl;
			if(type == Array){
				s[sl].ST[pos].type = t->second->second->kind;
				s[sl].ST[pos].rangel = t->second->first->first->value;
				s[sl].ST[pos].rangeu = t->second->first->second->value;
				s[sl].ST[pos].isarr = Array;
			}
                }
	}
}

void check (tree t)
{
        for (t; t != NULL; t = t->next){
                switch (t->kind) {
                        case Procedure: {
				sl++;
				newst();
                                handle_decls (t->second);
                                check (t->third);
                                printf ("\n");
                                prST ();
				popst();
				sl--;
                                break;
			}
                        case Assign: {
                                int pos = t->first->value;
				if(s[sl].ST[pos].isarr != LBrack){
                                if (s[sl].ST[pos].index == 0) {
                                        s[sl].ST[pos].index = pos;
                                        s[sl].ST[pos].type = Integer;
                                }
                                if (check_expr (t->second) != s[sl].ST[pos].type)
                                        fprintf (stderr, "Type error in assignment to identifier %s\n",
                                                                id_name(t->first->value));
				}
				else{
					pos = t->first->first->value;
					int t1 = check_expr(t->first->second);
					
					if(s[sl].ST[pos].index == 0){
						s[sl].ST[pos].index = pos;
						s[sl].ST[pos].type = Integer;
					}
					if(t1 != s[sl].ST[pos].type)
						fprintf(stderr, "Type error in assignment to array %s\n", id_name(t->first->first->value));
				}
                                break;
                        }
			case Declare: {
				sl++;
				newst();
				handle_decls (t->first);
				check (t->second);
				printf ("\n");
				prST();
				popst();
				sl--;
				break;	
			}
			case For: {
				sl++;
				newst();
				check_expr(t->first);
				int tstart = check_expr(t->second->first);
				int tend = check_expr(t->second->second);
				if((tstart != Integer) || (tend != Integer) || (tstart != tend))
					fprintf(stderr, "Type error in for loop\n");
				else{
					check(t->third);
				}
				printf("\n");
				prST();
				popst();
				top--;
				sl--;
				break;
			}
			case Exit: {
				int t1;
				if(t->first != NULL){
					t1 = check_expr(t->first);
					if(t1 != Boolean)
						fprintf(stderr, "Non-boolean condition in WHILE\n");
				}
				break;
			}
                        case If: {
                                if (check_expr (t->first) != Boolean)
                                        fprintf (stderr, "Non-boolean in condition to IF\n");
                                check (t->second);
				if(t->third != NULL)
                                	check (t->third);
                                break;
			}
			case Else: {
				check(t->first);
				break;
			}
                        default: {
                                fprintf (stderr, "You shouldn't be here; invalid statement %i\n", t->kind);
			}
		} 
	}
}

void newst(){
	int i;
	for(i = 0; i < (top + 1); i++){
		s[sl].ST[i] = s[sl-1].ST[i];
	}
}

void popst(){
	int i;
	for(i = 0; i < (top + 1); i++){
		s[sl].ST[i].index = 0;
	        s[sl].ST[i].type = NoType;
	        s[sl].ST[i].sl = 0;
	        s[sl].ST[i].rangel = 0;
	        s[sl].ST[i].rangeu = 0;
	 	s[sl].ST[i].isarr = 0;
	}
}
