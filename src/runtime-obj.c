int debug = 5;
#include <stdio.h>
#include <stdlib.h>
#include "include/table.h"
#include "include/var.h"
#include "include/cmp.h"
#include "include/stack.c"
#include "vm.c"
#include "api.h"



element_table_t * RT_Strings;
element_table_t * RT_Global;

typedef struct Local_tables_struct
{
	element_table_t * Local;
	struct Local_tables_struct * pre_tbl;
}Local_tables_t;

Local_tables_t * L_Head;

void push_local_table(element_table_t * table)
{
	Local_tables_t * tmp = malloc(sizeof(Local_tables_t));
	tmp->pre_tbl = L_Head;
	tmp->Local = table;
	L_Head = tmp;
}

void pop_local_table()
{
	L_Head = L_Head->pre_tbl;
}

int size , *code_size , *globals_count , *strings_count;

element_table_t * new_local_tbl(char * name , int count)
{
	static int loc_tbl = 0;
	//printf("LOCAL[%d]\n" , loc_tbl++ );
	element_table_t * tmp = malloc(sizeof(element_table_t));
	tmp->name = name;
	tmp->count = count;
	tmp->var = calloc( count , sizeof(element_table_t));
	return tmp;
}

#include "include/cmp.h"

void op_sgv(int index)
{
	RT_Global->var[index] = *op_pop();
}	

void op_lgv(int index)
{
	op_push(RT_Global->var[index]);
}
			
void op_slv(int index)
{
	L_Head->Local->var[index] = *op_pop();
}

void op_llv(int index)
{
	op_push(L_Head->Local->var[index]);
}


void op_llt(int index)
{
	push_local_table(new_local_tbl("Local" , index));
}

void op_capi(int index)
{
	api[index]();
}

var_union pop_val()
{
	element_var_t var = *op_pop();
	return var.val;
}

void dump_stack()
{

}

int init(/*int a_code_size , int a_globals_count , int strings_count)*/)
{
	RT_Global = malloc(sizeof(element_table_t));
	RT_Global->name = "RT_Global";
	//RT_Global->count = *globals_count;
	//RT_Global->var = calloc( *globals_count , sizeof(element_var_t));
	L_Head = malloc(sizeof(Local_tables_t));
	//build_strings_table();
	return 0;
}
