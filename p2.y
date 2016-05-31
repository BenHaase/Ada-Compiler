%{
#include "tree.h"
#include <stdlib.h>

tree root = NULL;
%}

%token <t> Boolean 1 Integer 2 True 3 False 4
%token <t> And 5 Array 6 Begin 7 Declare 8
%token <t> Else 9 Elsif 10 End 11 Exit 12
%token <t> For 13 If 14 In 15 Is 16
%token <t> Loop 17 Mod 18 Not 19 Of 20 Or 21
%token <t> Procedure 22 Then 23 When 24 While 25 Xor 26
%token <i> Ident 27 IntConst 28
%token <t> Eq 29 NEq 30 Lt 31 LtEq 32 Gt 33 GtEq 34
%token <t> Plus 35 Minus 36 Star 37 Slash 38
%token <t> LParen 39 RParen 40 LBrack 41 RBrack 42
%token <t> Assign 43 Dd 44 SemiC 45 Colon 46 Comma 47 NoType 48

%start program

%union { tree t; int i; }

%type <t>	decls declaration id_list type const_range stmts statement bool_expr
%type <t>	range ref end_if expr relation sum sign prod factor basic

%%
program
	: Procedure Ident Is decls Begin stmts End SemiC
		{root = buildTree(Procedure, buildIntTree(Ident, $2), $4, $6);}
decls
	:
		{ $$ = NULL; }
	| declaration SemiC decls
		{$1->next = $3; $$ = $1;}
	;
declaration
	: id_list Colon type
		{$$ = buildTree(Colon, $1, $3, NULL);}
	;
id_list
	: Ident
		{$$ = buildIntTree(Ident, $1);}
	| Ident Comma id_list
		{$$ = buildIntTree(Ident, $1); $$->next = $3;}
	;
type
	: Integer
		{$$ = buildTree(Integer, NULL, NULL, NULL);}
	| Boolean
		{$$ = buildTree(Boolean, NULL, NULL, NULL);}
	| Array LBrack const_range RBrack Of type
		{$$ = buildTree(Array, $3, $6, NULL);}
	;
const_range
	: IntConst Dd IntConst
		{$$ = buildTree(Dd, buildIntTree(IntConst, $1), buildIntTree(IntConst,$3), NULL);}
	;
stmts
	: 
		{$$ = NULL;}
	| statement SemiC stmts
		{$$ = $1; $$->next = $3;}
	;
statement
	: ref Assign expr
		{$$ = buildTree(Assign, $1, $3, NULL);}
	| Declare decls Begin stmts End
		{$$ = buildTree(Declare, $2, $4, NULL);}
	| For Ident In range Loop stmts End Loop
		{$$ = buildTree(For, buildIntTree(Ident, $2), $4, $6);}
	| Exit
		{$$ = NULL;}
	| Exit When bool_expr
		{$$ = buildTree(Exit, $3, NULL, NULL);}
	| If bool_expr Then stmts end_if
		{$$ = buildTree(If, $2, $4, $5);}
	;
bool_expr
	: expr
		{$$ = $1;}
	;
range
	: sum Dd sum
		{$$ = buildTree(Dd, $1, $3, NULL);}
	;
ref
	: Ident
		{$$ = buildIntTree(Ident, $1);}
	| Ident LBrack expr RBrack
		{$$ = buildTree(LBrack, buildIntTree(Ident, $1), $3, NULL);}
	;
end_if
	: End If
		{$$ = NULL;}
	| Else stmts End If
		{$$ = buildTree(Else, $2, NULL, NULL);}
	| Elsif bool_expr Then stmts end_if
		{$$ = buildTree(If, $2, $4, $5);} /*changed Elsif to If*/
	;
expr
	: relation Or relation
		{$$ = buildTree(Or, $1, $3, NULL);}
	| relation And relation
		{$$ = buildTree(And, $1, $3, NULL);}
	| relation Xor relation
		{$$ = buildTree(Xor, $1, $3, NULL);}
	| relation
		{$$ = $1;}
	;
relation
	: sum
		{$$ = $1;}
	| sum Eq sum
		{$$ = buildTree(Eq, $1, $3, NULL);}
	| sum NEq sum
		{$$ = buildTree(NEq, $1, $3, NULL);}
	| sum Lt sum
		{$$ = buildTree(Lt, $1, $3, NULL);}
	| sum LtEq sum
		{$$ = buildTree(LtEq, $1, $3, NULL);}
	| sum Gt sum
		{$$ = buildTree(Gt, $1, $3, NULL);}
	| sum GtEq sum
		{$$ = buildTree(GtEq, $1, $3, NULL);}
	;
sum
	: sign prod
		{$2->next = $1; $$ = $2;}
	| sum Plus prod
		{$$ = buildTree(Plus, $1, $3, NULL);}
	| sum Minus prod
		{$$ = buildTree(Minus, $1, $3, NULL);}
	;
sign
	: Plus
		{$$ = buildTree(Plus, NULL, NULL, NULL);}
	| Minus
		{$$ = buildTree(Minus, NULL, NULL, NULL);}
	| 
		{$$ = NULL;}
	;
prod
	: factor
		{$$ = $1;}
	| prod Star factor
		{$$ = buildTree(Star, $1, $3, NULL);}
	| prod Slash factor
		{$$ = buildTree(Slash, $1, $3, NULL);}
	| prod Mod factor
		{$$ = buildTree(Mod, $1, $3, NULL);}
	;
factor
	: Not basic
		{$$ = buildTree(Not, $2, NULL, NULL);}
	| basic
		{$$ = $1;}
	;
basic
	: ref
		{$$ = $1;}
	| LParen expr RParen
		{$$ = $2;}
	| IntConst
		{$$ = buildIntTree(IntConst, $1);}
	| True
		{$$ = buildTree(True, NULL, NULL, NULL);}
	| False
		{$$ = buildTree(False, NULL, NULL, NULL);}
	;
%%
