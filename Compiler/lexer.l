%option noyywrap
%{
# include "helper.h"
# include "parser.h"
# include <stdlib.h>
extern void yyerror(const char *msg); 
%}
%%
";" |
"(" |
"+" |
"-" |
"*" |
"/" |
")" { return yytext[0]; }
">" { yylval.fn = 1; return CMP; }
"<" { yylval.fn = 2; return CMP; }
"<>" { yylval.fn = 3; return CMP; }
"=" { yylval.fn = 4; return CMP; }
">=" { yylval.fn = 5; return CMP; }
"<=" { yylval.fn = 6; return CMP; }
"do" { return DO; }
"for" { return FOR; }
"while" {return WHILE; }
"done" {return DONE; }
"if" {return IF; }
"then" {return THEN; }
"else" {return ELSE; }
":=" {return ASN;}
X{0,3}(V?I{0,3}|I(X|V)) {yylval.d = RomanToInt(yytext); return NUMBER;}	// римское число
[XVI]+ { printf("line %d: lexical error: NotRoman:%s\n", yylineno, yytext); } // ошибочное римское число
[a-zA-Z][a-zA-Z0-9]* { yylval.s = lookup(yytext); return NAME; }	//идентификатор
0 {yylval.d = 0; return NUMBER;}	// ноль
[1-9][0-9]* {yylval.d = atoi(yytext); return NUMBER; }	// десятичное число
"//".*
\n { yylineno += 1;}		//увеличение номера строки для отчета об ошибках
[ \t] /* игнорируем пробелы и табуляцию */
. { printf("line %d: lexical error: Mystery character:%c\n", yylineno, *yytext); }
%%