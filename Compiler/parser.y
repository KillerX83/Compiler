%defines "parser.h"
%define parse.trace
%verbose
%define parse.error verbose
%initial-action
{
	yydebug = 0;
}
%{
# include <stdio.h>
# include <stdlib.h>
# include "helper.h"
extern int yylex();
extern int yylineno;
void yyerror(const char* msg);
extern FILE* wFile;
%}
%union {
struct ast *a;
double d;	//значение переменной
struct symbol *s; /* имя переменной */
int fn;	//тип оператора сравнения
}
/* объявление токенов */
%token <d> NUMBER
%token <s> NAME
%token FOR DO WHILE DONE IF THEN ELSE
%right ASN
%left <fn> CMP
%left '+' '-'
%left '*' '/'
%type <a> exp stmt
%start program
%%
program: /* ничего */
	| program stmt { compile($2); treefree($2);}
	;
stmt: FOR '(' exp ';' exp ';' exp ')' DO '(' exp ')' { $$ = newflow('F', $3, $5, $7, $11, NULL); }
	| WHILE '(' exp ')' exp DONE { $$ = newflow('W', NULL, $3, NULL, $5, NULL); }
	| IF exp THEN exp ELSE exp { $$ = newflow('E', NULL, $2, NULL, $4, $6); }
	| IF exp THEN exp { $$ = newflow('I', NULL, $2, NULL, $4, NULL); }
	| exp ';'
	;
exp: exp CMP exp { $$ = newcmp($2, $1, $3); }
	| NAME ASN exp { $$ = newasgn($1, $3); }
	| '(' exp ')' { $$ = $2; }
	| NUMBER { $$ = newnum($1); }
	| NAME { $$ = newref($1); }
	| exp '+' exp { $$ = newast('+', $1,$3); }
	| exp '-' exp { $$ = newast('-', $1,$3); }
	| exp '*' exp { $$ = newast('*', $1,$3); }
	| exp '/' exp { $$ = newast('/', $1,$3); }
	;
%%
void yyerror(const char* msg)
{
	printf("line %d: %s", yylineno, msg);
}
