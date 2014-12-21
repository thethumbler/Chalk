
#include <stdio.h>
#include <string.h>
#include "var.h"
#include "arithmetics.h"
#include "cmp.h"

#define MAX_STACK 256
element_var_t STACK[MAX_STACK];
int STACK_HEAD = 0;

void op_push(element_var_t var)
{
	STACK_HEAD++;
	STACK[STACK_HEAD].type = var.type;
	switch(var.type)
	{
		case  	INT: STACK[STACK_HEAD].val.int_t = var.val.int_t;
		case DOUBLE: STACK[STACK_HEAD].val.double_t = var.val.double_t;
		case STRING: STACK[STACK_HEAD].val.char_t = var.val.char_t;
	}
	if(debug>2)
	{
		printf("PUSH STACK[%d]: " , STACK_HEAD );
		switch(STACK[STACK_HEAD].type)
		{
			case 	INT: printf("%d\n" , STACK[STACK_HEAD].val.int_t ); break;
			case DOUBLE: printf("%f\n" , STACK[STACK_HEAD].val.double_t ); break;
			case STRING: printf("%s\n" , STACK[STACK_HEAD].val.char_t ); break;
				default: printf("(%d)\n" , STACK[STACK_HEAD].type ); break;
		}
	}
}

element_var_t * op_pop()
{
	return &STACK[STACK_HEAD--];
}

void op_add()
{
	element_var_t var;
	alu_add(op_pop(),op_pop(),&var);
	op_push(var);
}

void op_sub()
{
	element_var_t var;
	alu_sub(op_pop(),op_pop(),&var);
	op_push(var);
}

void op_mul()
{
	element_var_t var;
	alu_mul(op_pop(),op_pop(),&var);
	op_push(var);
}

void op_div()
{
	element_var_t var;
	alu_div(op_pop(),op_pop(),&var);
	op_push(var);
}

int op_tlt()
{
	return alu_tlt(op_pop() , op_pop());
}

int op_tgt()
{
	return alu_tgt(op_pop() , op_pop());
}

int op_tle()
{
	return alu_tle(op_pop() , op_pop());
}

int op_tge()
{
	return alu_tge(op_pop() , op_pop());
}

int op_teq()
{
	return alu_teq(op_pop() , op_pop());
}

int op_tne()
{
	return alu_tne(op_pop() , op_pop());
}

/*
int main(int argc, char const *argv[])
{
	element_var_t var1 = {.type = STRING , .val.char_t = "Hello, "};
	element_var_t var2 = {.type = STRING , .val.char_t = "World!" };
	op_push(&var1);
	op_push(&var2);
	element_var_t var3;
	add(op_pop(),op_pop(),&var3);
	op_push(&var3);
	printf("%s\n", op_pop()->val.char_t );
	return 0;
}
*/
