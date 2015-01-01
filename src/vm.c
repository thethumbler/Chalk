#ifndef _VM_C
#define _VM_C
#include "include/asm.h"

table_t * Global;
table_t * Local;
element_table_t * Strings;

int RET_STACK[MAX_STACK];
int RET_STACK_COUNTER = 0;

int init_int()
{
	Global = init_table("Global");
	//add_to_table( Global , new_var_t( "^_^" , NULL ));	//Dummy varialbe
	Strings = init_element_table("String");
	//add_to_table( Strings , new_var_t( "^_^" , NULL ));	//Dummy string
	return 0;
}
void new_local_table()
{
	Local = init_table("Local");
	//Local->var = malloc(sizeof(var_t));
	//add_to_table(Local , new_var_t( "^" , NULL ));	//Dummy variable .. impossible to be declared by the user
}

typedef enum 
{
	OP_NOP	,	/* No operation : Does nothing .. really =D */
	OP_HALT	,	/* Halt : Terminates the machine */
	OP_PUSH , 	/* Push a value on top of the stack */
	OP_POP 	,	/* Pop the top value from the stack */
	OP_LSTR	, 	/* Load a string value and push it on top of the stack */
	OP_ADD	,	/* Pop 2 values from the stack .. add them then push the result */
				/* NOTE : OP_ADD is overloaded .. it can add 2 numerical values or even concatinate 2 strings */
	OP_SUB	,	/* Pop 2 values from the stack .. subtract the first popped from the last then push the result */
	OP_MUL	,	/* Pop 2 values from the stack .. multiply them then push the result */
	OP_DIV  ,	/* Pop 2 values from the stack .. divide the last popped into the first then push the result */
	OP_POW  ,	/* Pop 2 values from the stack .. raise the last popped to the power of the first then push the result */
	OP_TLT	,	/* test if less than : Pop A , Pop B .. if B < A push 1 .. otherwise push 0 */
	OP_TLE	,	/* test if less or equal : Pop A , Pop B .. if B <= A push 1 .. otherwise push 0 */
	OP_TGT	,	/* test if greater than : Pop A , Pop B .. if B > A push 1 .. otherwise push 0 */
	OP_TGE	,	/* test if greater or equal : Pop A , Pop B .. if B >= A push 1 .. otherwise push 0 */
	OP_TEQ	,	/* test if equal : Pop A , Pop B .. if B = A push 1 .. otherwise push 0 */
	OP_TNE	,	/* test if not equal : Pop A , Pop B .. if B != A push 1 .. otherwise push 0 */
	OP_JZ	,	/* Jump if zero to specific location relative to PC */
	OP_JNZ	,	/* Jump if not zero to specific location relative to PC */
	OP_JMP 	,	/* Uncoditional jumping to specific location relative to PC */
	OP_CAPI	,	/* Calls a default API function then returns back to where it was before calling */
	OP_CALL	,	/* Calls a function then returns back to where it was before calling */
	OP_RET	,	/* Returns from a function to where it was before calling */
	OP_SGV	,	/* Pop a value and save it into variable in the Global table */
	OP_LGV	,	/* Load a value from the Global table and push it on the stack */
	OP_SLV	,	/* Pop a value and save it into variable in the Local table */
	OP_LLV	,	/* Load a value from the Local table and push it on the stack */
	OP_LLT	,	/* Construct a local table with the given elements count */
	OP_CT 	, 	/* Constract tabel .. pops table name .. initilaizes it */
	OP_LT 	,	/* Pushes the address of cur_table to the stack 	*/
	OP_LTE 	,	/* Load table element .. pops one elemnt from the stack .. the element name .. loads it ..
					then pushes the loaded element */
	OP_STE		/* Set table element .. pops two elements the first is the variable , 
				 	the second is it's name */
} op_t;

#endif