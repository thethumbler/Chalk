#include "include/var.h"
#include "include/asm.h"
#include "include/table.h"
#include <string.h>

element_var_t var;
char chunk[0xA];
/*
 * instructions are 10 bytes long	
 * |    0x0    |  0x1  ...  0x9  |
 * | operation |     operand     |
 */
int inst_count = 0;
int counting=0;
void _asm(op_t op , element_var_t * arg)	/* using _asm because asm is already reserved */
{
	inst_count++;
	empty(chunk,0xA);
	chunk[0] = op;
	switch(op)
	{
		case  OP_SGV:	arg->val.int_t = add_to_table(Global , new_var_t( arg->val.char_t , NULL )); break;
		case  OP_LGV:	dump_table(Global); arg->val.int_t = get_index_in_table(Global , arg->val.char_t); break;
		case  OP_SLV:	arg->val.int_t = add_to_table(Local , new_var_t( arg->val.char_t , NULL )); break;
		case  OP_LLV:	arg->val.int_t = get_index_in_table(Local , arg->val.char_t); break;
		case  OP_LSTR:	if(!counting)arg->val.int_t = add_to_table(Strings , new_var_t( NULL , arg )); break;
	}
	if(counting)return;

	if(arg != NULL) 
	{
		var_union tmp_var_union;
		chunk[1] = arg->type;
		memcpy(&tmp_var_union , &arg->val , (int)sizeof(var_union));
		memcpy(&chunk[2] , &tmp_var_union , 8);
	}
	if(debug>1)
	{
		printf("%d:\t" , chunk_buf_pos/10 );
		print_hex(chunk , 10);
		printf("\n");
	}
	write_chunk(chunk);
}
