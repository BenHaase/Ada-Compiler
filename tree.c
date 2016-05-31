#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#include "y.tab.h"

tree buildTree (int kind, tree one, tree two, tree three)
{
        tree p = (tree)malloc(sizeof (node));
        p->kind = kind;
        p->first = one;
        p->second = two;
        p->third = three;
        p->next = NULL;
        return p;
}

tree buildIntTree (int kind, int val)
{
        tree p = (tree)malloc(sizeof (node));
        p->kind = kind;
        p->value = val;
        p->first = p->second = p->third = NULL;
        p->next = NULL;
        return p;
}

char TokName[][12] =
        {"",
        "BOOLEAN", "INTEGER", "TRUE", "FALSE", "and", "array", "begin", "declare", "else", "elseif",
	"end", "exit", "for", "if", "in", "is", "loop", "mod", "not", "of",
	"or", "procedure", "then", "when", "while", "xor", "Ident", "IntConst", "=", "/=",
	"<", "<=", ">", ">=", "+", "-", "*", "/", "(", ")",
	"[", "]", ":=", "..", ";", ":", ",", "NoType"};
static int indent = 0;
void printTree (tree p)
{
        if (p == NULL) return;
        for (; p != NULL; p = p->next) {
                printf ("%*s%s", indent, "", TokName[p->kind]);
                switch (p->kind) {
                        case Ident:
                                printf ("  %s\n", id_name(p->value), p->value);
                                break;
                        case IntConst:
                                printf ("  %d\n", p->value);
                                break;
                        default:
                                printf ("\n");
                                indent += 2;
                                printTree (p->first);
                                printTree (p->second);
                                printTree (p->third);
                                indent -= 2;
                        }
                }
}
