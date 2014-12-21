#include <stdio.h>
#include <stdlib.h>

int debug = 0; 
void empty(char * buf , int size)
{
	while(buf[--size] = 0);
}

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

char * input;
int size , *code_size , *globals_count , *strings_count;

int check_header()
{
	/* Dummy function */
	return 0;
}

char * open_file(char * filename)
{
    FILE * file = fopen( filename , "r");
    fseek(file , 0L , SEEK_END );
    size = ftell(file);
    fseek(file , 0L , SEEK_SET);
    char * buf = calloc(size , sizeof(char));
    fread(buf , size , 1 , file);
    fclose(file);
    return buf;
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
	 	char * str = calloc(str_size+1,sizeof(char));	// Including the Null terminator 
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

void pr_push(element_var_t var)
{
	long * tmp = (long*)&var.val;
	switch(var.type)
	{
		case 	INT: printf("\t;PUSH %d\n\tmov edi , 0x1\n\tmov rsi , 0x%x\n\tcall new_var\n\tmov edi , DWORD [rax]\n\tmov rsi , QWORD [rax + 0x8]\n\tcall op_push\n" , var.val.int_t , var.val.int_t ); break;
		case DOUBLE: printf("\t;PUSH %f\n\tmov edi , 0x2\n\tmov rsi , 0x%lx\n\tcall new_var\n\tmov edi , DWORD [rax]\n\tmov rsi , QWORD [rax + 0x8]\n\tcall op_push\n" , var.val.double_t , *tmp ); break;
		
	}
}

void exec(char * prog , element_table_t * Str_tbl)
{
	RT_Global = malloc(sizeof(element_table_t));
	RT_Global->name = "RT_Global";
	RT_Global->count = 0;
	RT_Global->var = calloc(*strings_count , sizeof(element_var_t));
	int pc = 0;
	char * cur_chunk = calloc(10,sizeof(char));
	int running = 1;
	int c = 0;
	while(running)
	{
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
		printf("l%d:\n" , pc );
		switch(op)
		{
			case OP_HALT: 	printf("\t;HALT\n\tJMP exit\n"); running = 0; break;
			case OP_PUSH:	pr_push(tmp); break;
			case OP_SGV :	printf("\t;SGV %d\n\tMOV edi , 0x%x\n\tCALL op_sgv\n" , tmp.val.int_t , tmp.val.int_t); break;
			case OP_LSTR:	printf("\t;LSTR %d\n\tMOV edi , 0x%x\n\tCALL op_lstr\n" , tmp.val.int_t , tmp.val.int_t); break;
			case OP_LGV :	printf("\t;LGV %d\n\tMOV edi , 0x%x\n\tCALL op_lgv\n" , tmp.val.int_t , tmp.val.int_t); break;
			case OP_SLV :	printf("\t;SLV %d\n\tMOV edi , 0x%x\n\tCALL op_slv\n" , tmp.val.int_t , tmp.val.int_t); break;
			case OP_LLV :	printf("\t;LLV %d\n\tMOV edi , 0x%x\n\tCALL op_llv\n" , tmp.val.int_t , tmp.val.int_t); break;
			case OP_ADD :	printf("\t;ADD\n\tCALL op_add\n"); break;
			case OP_SUB :	printf("\t;SUB\n\tCALL op_sub\n"); break;
			case OP_MUL :	printf("\t;MUL\n\tCALL op_mul\n"); break;
			case OP_DIV :	printf("\t;DIV\n\tCALL op_div\n"); break;
			case OP_CALL:	printf("\t;CALL l%d\n\tCALL l%d\n" , tmp.val.int_t , tmp.val.int_t + 1); break;
			case OP_RET :	printf("\t;RET\n\tRET\n"); break;
			case OP_TLT :	printf("\t;TLT\n\tCALL op_tlt\n"); break;
			case OP_TLE	:	printf("\t;TLE\n\tCALL op_tle\n"); break;
			case OP_TGT	:	printf("\t;TGT\n\tCALL op_tgt\n"); break;
			case OP_TGE	:	printf("\t;TGE\n\tCALL op_tge\n"); break;
			case OP_TEQ	:	printf("\t;TEQ\n\tCALL op_teq\n"); break;
			case OP_TNE	:	printf("\t;TNE\n\tCALL op_tne\n"); break;
			case OP_JMP :	printf("\t;JMP %d\n\tJMP l%d\n" , tmp.val.int_t , pc + tmp.val.int_t + 1); break;
			case OP_JZ  :	printf("\t;JZ %d\n\tCALL pop_val\n\tCMP rax , 0x0\n\tJE l%d\n" , tmp.val.int_t , pc + tmp.val.int_t + 1); break;
			case OP_CAPI:	printf("\t;CAPI %d\n\tMOV edi , 0x%x\n\tCALL op_capi\n" , tmp.val.int_t , tmp.val.int_t); break;
			case OP_LLT :	printf("\t;LLT %d\n\tMOV edi , 0x%x\n\tCALL op_llt\n" , tmp.val.int_t , tmp.val.int_t); break;
		}	
		printf("\n");	
		pc++;
	}
}


int main()
{
	input = open_file(".out");
	check_header();
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
