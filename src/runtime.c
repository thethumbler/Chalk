#include <stdio.h>
#include <stdlib.h>
#include "include/table.h"
#include "include/var.h"
#include "include/cmp.h"
#include "stack.c"
#include "vm.c"
#include "include/api.h"
#include "include/lex.h"



element_table_t * RT_Strings;
element_table_t * Str_tbl;
element_table_t * RT_Global;
table_t * cur_table;

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

char * input;
int size , *code_size , *globals_count , *strings_count;

int check_header(char * src)
{
	if(!strncmp(src , "CHK" , 3) && src[3] == 0x21)
		return 1;
	return 0;
}

void build_strings_table()
{
	int slots[*strings_count];
	/* 
	 * Building Strings slots
	 */
	int i;
	for(i=0;i<*strings_count;i++)
	{
		slots[i] = (int)(*(input + 12 + *code_size + (i+1)*sizeof(int)));
	}

	/*
	 * Building Strings table
	 */
	 RT_Strings = malloc(sizeof(element_table_t));
	 RT_Strings->name = "RT_Strings";
	 RT_Strings->count = 0;
	 RT_Strings->var = calloc(*strings_count , sizeof(element_var_t));
	 
	 for(i=0;i<*strings_count;i++)
	 {
	 	int str_size = slots[i] - (i?slots[i-1]:0);
	 	char * str = _calloc(str_size+1,sizeof(char));	// Including the Null terminator 
	 	empty(str , str_size + 1);
	 	char * str_pos = input + 12 + *code_size + (*strings_count+1) * sizeof(int) + slots[i] - str_size;
	 	memcpy( str , str_pos , str_size );
	 	str[str_size] = '\0';
	 	RT_Strings->var[i].type = STRING;
	 	RT_Strings->var[i].val.char_t = str;
	 	//printf( "%s" , RT_Strings->var[i].val.char_t);
	 }
	 	
}

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

/**
 * For optimization 
 */

int pc = 0;
char * cur_chunk ;
int running = 1;
int c = 0;

op_t op ;

element_var_t tmp;
var_t vtmp;
var_union * val;



typedef void (*vm_op_func)(void);

void vm_op_nop(void)
{
	return;
}

void vm_op_halt(void)
{
 	running = 0; 
 	return;
}
void vm_op_push(void)	
{
	op_push(tmp); return;
}
void vm_op_sgv(void)	
{
	RT_Global->var[tmp.val.int_t] = *op_pop(); return;
}
void vm_op_lstr(void)	
{
	op_push(Str_tbl->var[tmp.val.int_t]); return;
}
void vm_op_lgv(void)	
{
	op_push(RT_Global->var[tmp.val.int_t]); return;
}
void vm_op_slv(void)	
{
	L_Head->Local->var[tmp.val.int_t] = *op_pop(); return;
}
void vm_op_llv(void)	
{
	op_push(L_Head->Local->var[tmp.val.int_t]); return;
}
void vm_op_add(void)	
{
	op_add(); return;
}
void vm_op_sub(void)	
{
	op_sub(); return;
}
void vm_op_mul(void)	
{
	op_mul(); return;
}
void vm_op_div(void)	
{
	op_div(); return;
}
void vm_op_pow(void)	
{
	op_pow(); return;
}
void vm_op_call(void)	
{
	RET_STACK[RET_STACK_COUNTER++] = pc; pc = tmp.val.int_t; return;
}
void vm_op_ret(void)	
{
	pop_local_table(); pc = RET_STACK[--RET_STACK_COUNTER]; return;
}
void vm_op_tlt(void)	
{
	op_tlt(); return;
}
void vm_op_tle(void)	
{
	op_tle(); return;
}
void vm_op_tgt(void)	
{
	op_tgt(); return;
}
void vm_op_tge(void)	
{
	op_tge(); return;
}
void vm_op_teq(void)	
{
	op_teq(); return;
}
void vm_op_tne(void)	
{
	op_tne(); return;
}
void vm_op_jmp(void)	
{
	pc += tmp.val.int_t; return;
}
void vm_op_jz(void)	
{
	if(!(op_pop()->val.int_t))pc += tmp.val.int_t; return;
}
void vm_op_capi(void)	
{
	api[tmp.val.int_t](); return;
}
void vm_op_llt(void)	
{
	push_local_table(new_local_tbl("Local" , val->int_t)); return;
}
void vm_op_ct(void)	
{
	cur_table = init_table("Table");
	cur_table->var = malloc(sizeof(var_t));
}
void vm_op_lt(void)	
{
	op_push(*new_var( TABLE , (var_union)cur_table));
}
void vm_op_lte(void)	
{
	op_push(get_from_table(cur_table , op_pop()->val.char_t).var);
}
void vm_op_ste(void)	
{
	var_t * var = malloc(sizeof(var_t));
	var->name = op_pop()->val.char_t;
	var->var = *op_pop();
	add_to_table(cur_table , var);
}

vm_op_func vm_op[] = 
{
	vm_op_nop  	, 
	vm_op_halt 	,
	vm_op_push 	,
	NULL , 	/* vm_op_pop	, */
	vm_op_lstr	,
	vm_op_add	,
	vm_op_sub	,
	vm_op_mul	,
	vm_op_div	,
	vm_op_pow	,
	vm_op_tlt	,
	vm_op_tle	,
	vm_op_tgt	,
	vm_op_tge	,
	vm_op_teq	,
	vm_op_tne	,
	vm_op_jz	,
	NULL ,	/* vm_op_jnz	, */
	vm_op_jmp	,
	vm_op_capi	,
	vm_op_call	,
	vm_op_ret	,
	vm_op_sgv	,
	vm_op_lgv	,
	vm_op_slv	,
	vm_op_llv	,
	vm_op_llt	,
	vm_op_ct	,
	vm_op_lt 	,
	vm_op_lte 	,
	vm_op_ste	
};


void exec(char * prog , element_table_t * Str_tbl_in)
{
	Str_tbl = Str_tbl_in;
	/*
	int pc = 0;
	char * cur_chunk = calloc(10,sizeof(char));
	int running = 1;
	int c = 0;
	*/
	int c = 0;
	while(running)
	{
		c++;
		cur_chunk = calloc(10,sizeof(char));

		empty(cur_chunk,0xA);
		memcpy(cur_chunk,&prog[10*pc],10);
		if(debug>3)
		{
			printf("@%d\t" , pc);
			print_hex(cur_chunk,10);
			printf("\n");
		}
		op = cur_chunk[0];
		tmp.type = cur_chunk[1];
		memcpy(&tmp.val , &cur_chunk[2] , (int)sizeof(var_union));
		val = (var_union*)&cur_chunk[2];

		//printf("OP = %d\n" , op );
		vm_op[op]();
		/*
		c++;
		empty(cur_chunk,0xA);
		memcpy(cur_chunk,&prog[10*pc],10);
		if(debug>3)
		{
			printf("@%d\t" , pc);
			print_hex(cur_chunk,10);
			printf("\n");
		}
		op_t op = cur_chunk[0];
		element_var_t tmp;
		var_t vtmp;
		tmp.type = cur_chunk[1];
		memcpy(&tmp.val , &cur_chunk[2] , (int)sizeof(var_union));
		var_union * val = (var_union*)&cur_chunk[2];
		//printf("@%d:\tOP=%x\tTYPE=%x\tARG=%lx\n" , pc , op , cur_chunk[1] , (long)(cur_chunk[2]) );
		//if(c>50)exit(0);
		
		switch(op)
		{
			case OP_HALT: 	running = 0; break;
			case OP_PUSH:	op_push(tmp); break;
			case OP_SGV :	RT_Global->var[tmp.val.int_t] = *op_pop(); break;
			case OP_LSTR:	op_push(Str_tbl->var[tmp.val.int_t]); break;
			case OP_LGV :	op_push(RT_Global->var[tmp.val.int_t]); break;
			case OP_SLV :	L_Head->Local->var[tmp.val.int_t] = *op_pop(); break;
			case OP_LLV :	op_push(L_Head->Local->var[tmp.val.int_t]); break;
			case OP_ADD :	op_add(); break;
			case OP_SUB :	op_sub(); break;
			case OP_MUL :	op_mul(); break;
			case OP_DIV :	op_div(); break;
			case OP_POW :	op_pow(); break;
			case OP_CALL:	RET_STACK[RET_STACK_COUNTER++] = pc; pc = tmp.val.int_t; break;
			case OP_RET :	pop_local_table(); pc = RET_STACK[--RET_STACK_COUNTER]; break;
			case OP_TLT :	op_tlt(); break;
			case OP_TLE	:	op_tle(); break;
			case OP_TGT	:	op_tgt(); break;
			case OP_TGE	:	op_tge(); break;
			case OP_TEQ	:	op_teq(); break;
			case OP_TNE	:	op_tne(); break;
			case OP_JMP :	pc += tmp.val.int_t; break;
			case OP_JZ  :	if(!(op_pop()->val.int_t))pc += tmp.val.int_t; break;
			case OP_CAPI:	api[tmp.val.int_t](); break;
			case OP_LLT :	push_local_table(new_local_tbl("Local" , val->int_t)); break;
		}		
		*/
		pc++;
	}
}


int launch_exec(char * src)
{
	input = src;
	code_size = (int*)(input+4);
	globals_count = (int*)((input+8));
	strings_count = (int*)((input+12+*code_size));
	//printf("C=%d\nG=%d\nS=%d\n" , *code_size , *globals_count , *strings_count );
	RT_Global = malloc(sizeof(element_table_t));
	RT_Global->name = "RT_Global";
	RT_Global->count = *globals_count;
	RT_Global->var = calloc( *globals_count , sizeof(element_var_t));
	L_Head = malloc(sizeof(Local_tables_t));
	init_int();
	build_strings_table();
	exec( input + 12 , RT_Strings);
	return 0;
}

/*
void build_strings_element_table(table_t * strings)
{
	/*
	 * Building Strings table
	 
	 RT_Strings = malloc(sizeof(element_table_t));
	 RT_Strings->name = "RT_Strings";
	 RT_Strings->count = strings->count;
	 RT_Strings->var = calloc(strings->count , sizeof(element_var_t));
	 int i;
	 for(i=0;i<strings->count;i++)
	 {
	 	RT_Strings->var[i] = strings->var[i].var;
	 }
	 	
}
*/

void launch(char * src , int globals_count , element_table_t * strings , table_t * globals /* Optional debug data*/ )
{
	L_Head = malloc(sizeof(Local_tables_t));
	RT_Global = malloc(sizeof(element_table_t));
	RT_Global->name = "RT_Global";
	RT_Global->count = globals_count;
	RT_Global->var = calloc( globals_count , sizeof(element_var_t));
	//build_strings_element_table(strings);
	exec(src , strings);
}
