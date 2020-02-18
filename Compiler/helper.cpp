#define _CRT_SECURE_NO_WARNINGS
/*
* функции из файла helper.h
*/
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <math.h>
# include "helper.h"

extern FILE* yyin;
FILE* wFile;
int lbl;
/* таблица символов */
#define NHASH 9997
struct symbol symtab[NHASH];
/* хэширование */
static unsigned
symhash(char* sym)
{
	unsigned int hash = 0;
	unsigned c;
	while (c = *sym++) hash = hash * 9 ^ c;
	return hash;
}
struct symbol*
	lookup(char* sym)
{
	struct symbol* sp = &symtab[symhash(sym) % NHASH];
	sp->nodetype = 'S';
	int scount = NHASH; 
	while (--scount >= 0) {
		if (sp->name && !strcmp(sp->name, sym)) { return sp; }
		if (!sp->name) { /* добавление нового элемента */
			sp->name = _strdup(sym);
			sp->value = 0;
			return sp;
		}
		if (++sp >= symtab + NHASH) sp = symtab; 
	}
	yyerror("symbol table overflow\n");
	abort(); /* таблица заполнена */
}
struct ast*
	newast(int nodetype, struct ast* l, struct ast* r)
{
	struct ast* a = (struct ast*)malloc(sizeof(struct ast));
	if (!a) {
		yyerror("out of space");
		exit(0);
	}
	a->nodetype = nodetype;
	a->l = l;
	a->r = r;
	return a;
}
struct ast*
	newnum(int d)
{
	struct numval* a = (struct numval*)malloc(sizeof(struct numval));
	if (!a) {
		yyerror("out of space");
		exit(0);
	}
	a->nodetype = 'K';
	a->number = d;
	return (struct ast*)a;
}
struct ast*
	newcmp(int cmptype, struct ast* l, struct ast* r)
{
	struct ast* a = (struct ast*)malloc(sizeof(struct ast));
	if (!a) {
		yyerror("out of space");
		exit(0);
	}
	a->nodetype = '0' + cmptype;
	a->l = l;
	a->r = r;
	return a;
}

struct ast*
	newref(struct symbol* s)
{
	struct symref* a = (struct symref*)malloc(sizeof(struct symref));
	if (!a) {
		yyerror("out of space");
		exit(0);
	}
	a->nodetype = 'N';
	a->s = s;
	return (struct ast*)a;
}
struct ast*
	newasgn(struct symbol* s, struct ast* v)
{
	struct symasgn* a = (struct symasgn*)malloc(sizeof(struct symasgn));
	if (!a) {
		yyerror("out of space");
		exit(0);
	}
	a->nodetype = 'A';
	a->s = s;
	a->v = v;
	return (struct ast*)a;
}
struct ast*
	newflow(int nodetype, struct ast* start, struct ast* cond, struct ast* step, struct ast* tbranch, struct ast* fbranch)
{
	struct flow* a = (struct flow*)malloc(sizeof(struct flow));
	if (!a) {
		yyerror("out of space");
		exit(0);
	}
	a->nodetype = nodetype;
	a->start = start;
	a->cond = cond;
	a->step = step;
	a->tbranch = tbranch;
	a->fbranch = fbranch;
	return (struct ast*)a;
}
/* освобождение выделенной пам€ти */
void
treefree(struct ast* a) 
{
	switch (a->nodetype) {
		/* два поддерева */
	case '-':
	case '+':
	case '*':
	case '/':
	case '1': case '2': case '3': case '4': case '5': case '6':
		treefree(a->r);
	case 'K': case 'N':
		break;
	case 'A':
		free(((struct symasgn*)a)->v);
		break;
		/* больше двух поддеревьев */
	case 'F':
	case 'W':
	case 'I':
	case 'E':
		free(((struct flow*)a)->cond);
		free(((struct flow*)a)->start);
		free(((struct flow*)a)->step);
		free(((struct flow*)a)->tbranch);
		free(((struct flow*)a)->fbranch);
		break;
	default: printf("internal error: free bad node %c\n", a->nodetype);
	}
	free(a); /* всегда удал€ем сам узел */
}

void compile(struct ast* a)
{
	// действуем в зависимости от типа узла и от типов дочерних узлов
	// так же при трансл€ции активно используетс€ стэк дл€ соблюдени€
	// приоритета операторов
	switch (a->nodetype)
	{
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
		switch (a->l->nodetype)
		{
		case 'K':
		case 'N':
			fprintf(wFile, "\npush ");
			compile(a->l);
			break;
		default:
			compile(a->l);
			break;

		}
		switch (a->r->nodetype)
		{
		case 'K':
		case 'N':
			fprintf(wFile, "\npush ");
			compile(a->r);
			break;
		default:
			compile(a->r);
			break;
		}
		fprintf(wFile, "\npop bx");
		fprintf(wFile, "\npop cx");
		fprintf(wFile, "\ncmp cx, bx");
		int lbl1, lbl2;
		switch (a->nodetype)
		{
		case '1':
			fprintf(wFile, "\njg L%d", lbl1 = lbl++);
			break;
		case '2':
			fprintf(wFile, "\njl L%d", lbl1 = lbl++);
			break;
		case '3':
			fprintf(wFile, "\njne L%d", lbl1 = lbl++);
			break;
		case '4':
			fprintf(wFile, "\nje L%d", lbl1 = lbl++);
			break;
		case '5':
			fprintf(wFile, "\njge L%d", lbl1 = lbl++);
			break;
		case '6':
			fprintf(wFile, "\njle L%d", lbl1 = lbl++);
			break;
		}
		fprintf(wFile, "\npush 0");
		fprintf(wFile, "\njmp L%d", lbl2 = lbl++);
		fprintf(wFile, "\nL%d:", lbl1);
		fprintf(wFile, "\npush 1");
		fprintf(wFile, "\nL%d:", lbl2);
		break;
	case '+':
		switch (a->l->nodetype)
		{
		case 'K':
		case 'N':
			fprintf(wFile, "\npush ");
			compile(a->l);
			break;
		default:
			compile(a->l);
			break;

		}
		switch (a->r->nodetype)
		{
		case 'K':
		case 'N':
			fprintf(wFile, "\npush ");
			compile(a->r);
			break;
		default:
			compile(a->r);
			break;

		}
		fprintf(wFile, "\npop bx");
		fprintf(wFile, "\npop cx");
		fprintf(wFile, "\nadd cx, bx");
		fprintf(wFile, "\npush cx");

		break;
	case '-':
		switch (a->l->nodetype)
		{
		case 'K':
		case 'N':
			fprintf(wFile, "\npush ");
			compile(a->l);
			break;
		default:
			compile(a->l);
			break;
		}
		switch (a->r->nodetype)
		{
		case 'K':
		case 'N':
			fprintf(wFile, "\npush ");
			compile(a->r);
			break;
		default:
			compile(a->r);
			break;

		}
		fprintf(wFile, "\npop bx");
		fprintf(wFile, "\npop cx");
		fprintf(wFile, "\nsub cx, bx");
		fprintf(wFile, "\npush cx");
		break;
	case '/':
		switch (a->l->nodetype)
		{
		case 'K':
		case 'N':
			fprintf(wFile, "\npush ");
			compile(a->l);
			break;
		default:
			compile(a->l);
			break;

		}
		switch (a->r->nodetype)
		{
		case 'K':
		case 'N':
			fprintf(wFile, "\npush ");
			compile(a->r);
			break;
		default:
			compile(a->r);
			break;

		}
		fprintf(wFile, "\npop cx");
		fprintf(wFile, "\npop ax");
		fprintf(wFile, "\ndiv cx");
		fprintf(wFile, "\nmov cx, ax");
		fprintf(wFile, "\npush cx");
		break;
	case '*':
		switch (a->l->nodetype)
		{
		case 'K':
		case 'N':
			fprintf(wFile, "\npush ");
			compile(a->l);
			break;
		default:
			compile(a->l);
			break;

		}
		switch (a->r->nodetype)
		{
		case 'K':
		case 'N':
			fprintf(wFile, "\npush ");
			compile(a->r);
			break;
		default:
			compile(a->r);
			break;

		}
		fprintf(wFile, "\npop cx");
		fprintf(wFile, "\npop ax");
		fprintf(wFile, "\nmul cx");
		fprintf(wFile, "\nmov cx, ax");
		fprintf(wFile, "\npush cx");

		break;
	

	case 'A':
		
		if (((struct symasgn*)a)->v->nodetype == 'N' || ((struct symasgn*)a)->v->nodetype == 'K')
		{
			fprintf(wFile, "\nmov cx, ");
			compile(((struct symasgn*)a)->v);
			fprintf(wFile, "\nmov ");
			compile(((struct ast*)((struct symref*)a)->s));
			fprintf(wFile, ", cx");
			break;
		}
		compile(((struct symasgn*)a)->v);
		fprintf(wFile, "\npop cx");
		fprintf(wFile, "\nmov ");
		compile(((struct ast*)((struct symasgn*)a)->s));
		fprintf(wFile, ", cx");
		break;
		
	case 'F':
		compile(((struct flow*)a)->start);
		fprintf(wFile, "\nL%d:", lbl1 = lbl++);
		compile(((struct flow*)a)->cond);
		fprintf(wFile, "\npop cx");
		fprintf(wFile, "\ncmp cx, 0");
		fprintf(wFile, "\nje L%d", lbl2 = lbl++);
		compile(((struct flow*)a)->tbranch);
		compile(((struct flow*)a)->step);
		fprintf(wFile, "\njmp L%d", lbl1);
		fprintf(wFile, "\nL%d:", lbl2);
		break;
	case 'E':
		compile(((struct flow*)a)->cond);
		fprintf(wFile, "\npop cx");
		fprintf(wFile, "\ncmp cx, 0");
		fprintf(wFile, "\nje L%d", lbl1 = lbl++);
		compile(((struct flow*)a)->tbranch);
		fprintf(wFile, "\njmp L%d", lbl2 = lbl++);
		fprintf(wFile, "\nL%d:", lbl1);
		compile(((struct flow*)a)->fbranch);
		fprintf(wFile, "\nL%d:", lbl2);
		break;
	case 'I':
		compile(((struct flow*)a)->cond);
		fprintf(wFile, "\npop cx");
		fprintf(wFile, "\ncmp cx, 0");
		fprintf(wFile, "\nje L%d", lbl2 = lbl++);
		fprintf(wFile, "\nL%d:", lbl1 = lbl++);
		compile(((struct flow*)a)->tbranch);
		fprintf(wFile, "\nL%d:", lbl2);
		break;
	case 'W':
		fprintf(wFile, "\nL%d:", lbl1 = lbl++);
		compile(((struct flow*)a)->cond);
		fprintf(wFile, "\npop cx");
		fprintf(wFile, "\ncmp cx, 0");
		fprintf(wFile, "\nje L%d", lbl2 = lbl++);
		compile(((struct flow*)a)->tbranch);
		fprintf(wFile, "\njmp L%d", lbl1);
		fprintf(wFile, "\nL%d:", lbl2);
		break;
	case 'S':
		fprintf(wFile, "%s", ((struct symbol*)a)->name);
		break;
	case 'N':
		fprintf(wFile, "%s", ((struct symref*)a)->s->name);
		break;
	case 'K':
		fprintf(wFile, "%d", ((struct numval*)a)->number);
		break;
	}
}

int RomanToInt(const char* Roman)
{
	int i = 0;
	int result = 0;
	while (i < strlen(Roman))
	{
		if (Roman[i] == 'X')
		{
			result += 10;
			i++;
			continue;
		}
		else if (Roman[i] == 'V')
		{
			result += 5;
			i++;
			while (i < strlen(Roman))
			{
				result++;
				i++;
			}
			return result;
		}
		else if (Roman[i] == 'I')
		{
			result++;
			i++;
			if (Roman[i] == 'V')
			{
				result += 3;
			}
			else if (Roman[i] == 'X')
			{
				result += 8;
			}
			else
			{
				while (i < strlen(Roman))
				{
					result++;
					i++;
				}
			}
			return result;
		}
	}
}

int
main(int argc, char* argv[])
{
	wFile = fopen(argv[1], "w");
	yyin = fopen(argv[2], "r");
	if (yyin == NULL)
	{
		printf("No such file or directory: %s", argv[2]);
		return 1;
	}
	fprintf(wFile, "\norg 100h");

	yyparse();

	fprintf(wFile, "\nret");
	
	for (size_t i = 0; i < NHASH; i++)
	{
		if (symtab[i].name != NULL)
		{
			fprintf(wFile, "\n%s dw ?", symtab[i].name);
		}
	}
	return 0;
}
