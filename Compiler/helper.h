#pragma once
// важные данные из других файлов
extern int yylineno; 
void yyerror(const char* msg);
int yyparse();

struct symbol { // имя переменной
	int nodetype;
	char* name;
	double value;
};
// функция поиска/добавления переменной в таблицу символов

struct symbol* lookup(char*);


/* типы узлов синтаксического дерева
* 1-6 - операторы сравнения
* L список инструкций (для тела цикла)
* N ссылка на таблицу символов
* S идентификатор
* A присваивание
*/
/* узлы синтаксического дерева */
/* все узлы имеют базовый тип */
struct ast {
	int nodetype;
	struct ast* l;
	struct ast* r;
};
struct flow {
	int nodetype; /* тип F */
	struct ast* start; /* стартовое выражение */
	struct ast* cond; /* условие */
	struct ast* step; /* выражение в конце итерации */
	struct ast* tbranch; /* тело цикла */
	struct ast* fbranch; /* альтернативная ветка */
	
};
struct numval {
	int nodetype; /* тип K */
	int number;
};
struct symref {
	int nodetype; /* тип N */
	struct symbol* s;
};
struct symasgn {
	int nodetype; /* тип A */
	struct symbol* s;
	struct ast* v; /* значение */
};
/* построение синтаксического дерева */
struct ast* newast(int nodetype, struct ast* l, struct ast* r);
struct ast* newcmp(int cmptype, struct ast* l, struct ast* r);
struct ast* newref(struct symbol* s);
struct ast* newasgn(struct symbol* s, struct ast* v);
struct ast* newnum(int d);
struct ast* newflow(int nodetype, struct ast* start, struct ast* cond, struct ast* step, struct ast* tbranch, struct ast* fbranch);
// трансляция
void compile(struct ast*);
/* освобождение памяти */
void treefree(struct ast*);
// перевод римского числа в десятичное
int RomanToInt(const char* Roman);
