%{
#include <stdio.h>
#include "tree.h"
#include "y.tab.h"
void yyerror(char *s);
int line=1;
int pos = 1;
%}
L	[A-Za-z]
D	[0-9]
%%
[ \t]+		;
[\n]		line++;
BOOLEAN		{return Boolean;}
INTEGER		{return Integer;}
TRUE		{return True;}
FALSE		{return False;}
and		{return And;}
array		{return Array;}
begin		{return Begin;}
declare		{return Declare;}
else		{return Else;}
elsif		{return Elsif;}
end		{return End;}
exit		{return Exit;}
for		{return For;}
if		{return If;}
in		{return In;}
is		{return Is;}
loop		{return Loop;}
mod		{return Mod;}
not		{return Not;}
of		{return Of;}
or		{return Or;}
procedure	{return Procedure;}
then		{return Then;}
when		{return When;}
while		{return While;}
xor		{return Xor;}
{L}({L}|{D}|"_")*	{yylval.i = lookup(yytext); return Ident;}
{D}*		{yylval.i = atoi(yytext); return IntConst;}
"="             {return Eq;}
"/="            {return NEq;}
"<"             {return Lt;}
"<="            {return LtEq;}
">"             {return Gt;}
">="            {return GtEq;}
"+"             {return Plus;}
"-"             {return Minus;}
"*"             {return Star;}
"/"             {return Slash;}
"("             {return LParen;}
")"             {return RParen;}
"["             {return LBrack;}
"]"             {return RBrack;}
":="            {return Assign;}
".."            {return Dd;}
";"             {return SemiC;}
":"             {return Colon;}
","             {return Comma;}
.		{yyerror("Bad Character");}
%%
int yywrap(void){return 1;}

void yyerror(char *s){printf("Error: %s at token %s in line %d\n", s, yytext, line);}

typedef char name[15];
static char Names[200][15] = {"<no name>"};
int top = 0;

int lookup (char s[])
{
        int     i;

        for (i = 1; i <= top; i ++)
                if (strcmp (s, Names[i]) == 0) return i;
        strcpy (Names[++top], s);
        return top;
}

void printNames (void)
{
        int i = 0;

        for (; i <= top; i++)
                printf ("%d\t%s\n", i, Names[i]);
}

char *id_name (int i)
{
        return Names[i];
}

void addname(int p, int n){
	strcpy(Names[p], Names[n]);
}
