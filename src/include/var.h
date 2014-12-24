#include <stdio.h>
#include <stdlib.h>

#ifndef _VAR_H
#define _VAR_H

typedef struct table_struct table_t;
typedef struct element_table_struct element_table_t;
typedef struct var_struct var_t;
typedef struct element_var_struct element_var_t;

typedef enum 
{
	NULL_T 	,
	INT 	,
	DOUBLE 	,
	STRING 	,
	TABLE
}data_t;

typedef union 
{
	int 	int_t;
	double 	double_t;
	char 	* char_t;
	table_t * table;
	var_t * var_ptr;
	element_var_t * evar_ptr;
}var_union;

typedef struct element_var_struct
{
	data_t type;
	var_union val;
}element_var_t;

typedef struct var_struct
{
	char * name;
	element_var_t var;
}var_t;

struct table_struct
{
	char * name;
	int count;
	var_t * var;	
};

struct element_table_struct
{
	char * name;
	int count;
	element_var_t * var;	
};


void print_var(var_t var)
{
    if(var.var.type == INT)
		printf("%s\tINT\t%d\n", var.name , var.var.val.int_t );
	else if (var.var.type == DOUBLE)
		printf("%s\tDOUBLE\t%f\n", var.name , var.var.val.double_t );
	else if (var.var.type == STRING)
		printf("%s\tSTRING\t%s\n", var.name , var.var.val.char_t  );
	else if (var.var.type == TABLE)
		printf("%s\tTABLE\t(N/A)\n", var.name );
}
element_var_t * new_var(data_t type , var_union val)
{
	element_var_t * var = _calloc(1,sizeof(element_var_t));
	var->type = type;
	switch(type)
	{
		case 	INT:	var->val.int_t = val.int_t; break;
		case DOUBLE:	var->val.double_t = val.double_t; break;
		case STRING:	var->val.char_t = val.char_t; break;
		case  TABLE:	var->val.table = val.table; break;
	}
	return var;
}

var_t * new_var_t(char * name , element_var_t * e_var)
{
	var_t * var = _calloc(1,sizeof(var_t));
	var->name = name;
	if(e_var)var->var = *e_var;
	return var;
}
#endif