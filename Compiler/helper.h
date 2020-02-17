#pragma once
// ������ ������ �� ������ ������
extern int yylineno; 
void yyerror(const char* msg);
int yyparse();

struct symbol { // ��� ����������
	int nodetype;
	char* name;
	double value;
};
// ������� ������/���������� ���������� � ������� ��������

struct symbol* lookup(char*);


/* ���� ����� ��������������� ������
* 1-6 - ��������� ���������
* L ������ ���������� (��� ���� �����)
* N ������ �� ������� ��������
* S �������������
* A ������������
*/
/* ���� ��������������� ������ */
/* ��� ���� ����� ������� ��� */
struct ast {
	int nodetype;
	struct ast* l;
	struct ast* r;
};
struct flow {
	int nodetype; /* ��� F */
	struct ast* start; /* ��������� ��������� */
	struct ast* cond; /* ������� */
	struct ast* step; /* ��������� � ����� �������� */
	struct ast* tbranch; /* ���� ����� */
	struct ast* fbranch; /* �������������� ����� */
	
};
struct numval {
	int nodetype; /* ��� K */
	int number;
};
struct symref {
	int nodetype; /* ��� N */
	struct symbol* s;
};
struct symasgn {
	int nodetype; /* ��� A */
	struct symbol* s;
	struct ast* v; /* �������� */
};
/* ���������� ��������������� ������ */
struct ast* newast(int nodetype, struct ast* l, struct ast* r);
struct ast* newcmp(int cmptype, struct ast* l, struct ast* r);
struct ast* newref(struct symbol* s);
struct ast* newasgn(struct symbol* s, struct ast* v);
struct ast* newnum(int d);
struct ast* newflow(int nodetype, struct ast* start, struct ast* cond, struct ast* step, struct ast* tbranch, struct ast* fbranch);
// ����������
void compile(struct ast*);
/* ������������ ������ */
void treefree(struct ast*);
// ������� �������� ����� � ����������
int RomanToInt(const char* Roman);
