#include "var.h"
#include "arithmetics.h"
#include <string.h>

#ifndef TABLE_C
#define TABLE_C

/* Ok .. let's take a little about tables .. 
 *
 * a table consists of a name, count and number of elements each can very in type and each has a unique name or id
 * each element contains these:
 *		-> name 	(char *) 
 * 		-> type 	(data_t)
 * 		-> value 	(var_union)
 *
 * element size = 2*ptr + 1 bytes 	where ptr depends on the platform (usually 4 bytes for 32-bit and 8 bytes for 64-bit )
 * that means usually an element is 9 bytes for 32-bit and 17 bytes for 64-bit
 *
 * Tables should be indexed by (and only by) names of elements .. this ensures safety and consistancy 
 *
 * Tables are allocated in blocks where the minmum number of elemnts in a block is defined by BASIC_BLOCK_SIZE
 *
 * When a certain block runs out of free elements another block is allocated and (for performance) the previous block
 * is never copied to another place but rather linked to the next block like a linked list
 *
 * To get element in place n (non-negative integer) .. this algorithem is performed:
 * 		starting with cur = the position of first element in the table
 * 		and counter = n/BASIC_BLOCK_SIZE (integer division)
 * 		while(counter--) cur = &(*(cur + BASIC_BLOCK_SIZE)->var.val.var_ptr); 
		cur += i % BASIC_BLOCK_SIZE;
		now E(n) = cur
 */

#define BASIC_BLOCK_SIZE	32

table_t * init_table(char * name)
{
	table_t * tmp = malloc(sizeof(table_t));
	tmp->name = name;
	tmp->count = 0;
	tmp->var = calloc( BASIC_BLOCK_SIZE + 1 , sizeof(var_t));
	return tmp;
}

element_table_t * init_element_table(char * name)
{
	element_table_t * tmp = malloc(sizeof(element_table_t));
	tmp->name = name;
	tmp->count = 0;
	tmp->var = _calloc( BASIC_BLOCK_SIZE + 1 , sizeof(element_var_t));
	return tmp;
}

/*
int table_index(table_t * table , int index ,  element_var_t * var)
{
	var_t * cur = table->var;
	int counter = index / BASIC_BLOCK_SIZE;
	while(counter--) cur = *(cur + 32); + index % BASIC_BLOCK_SIZE;
	table->var[index].var.type = var->type;
	
	/*
	if(var->type == INT)
		table->var[index].var.val.int_t = var->val.int_t;
	else if(var->type == DOUBLE)
		table->var[index].var.val.double_t = var->val.double_t;
	else if(var->type == STRING)	
		table->var[index].var.val.char_t = var->val.char_t;
	else if(var->type == TABLE)
		table->var[index].var.val.table = var->val.table;
	
	return table->count++;
}
*/

int add_to_table(table_t * table , var_t * var )
{
	/*
	 * Checking if the table already has this member
	 */
	int i;
	var_t * cur = table->var;
	if(var->name)
		for(i=0;i<table->count;i++)
		{
			cur = table->var;
			int counter = i / BASIC_BLOCK_SIZE;
			while(counter--) cur = &(*(cur + BASIC_BLOCK_SIZE)->var.val.var_ptr); 
			cur += i % BASIC_BLOCK_SIZE;
			if( !strcmp( (cur->name)?(cur->name):"\0" , var->name) && strlen(cur->name) == strlen(var->name)) break;
		}	
	/*
	else
	{
		i = table->count;	//Otherwise adding by index
		int counter = i / BASIC_BLOCK_SIZE;
		while(counter--) cur = &(*(cur + 32)->var.val.var_ptr); + i % BASIC_BLOCK_SIZE;
	}
	*/
	

	if(i == table->count) table->count++;
	cur = table->var;
	int counter = i / BASIC_BLOCK_SIZE;
	if( i && !(i % BASIC_BLOCK_SIZE) ) 
	{
		while(--counter) cur = &(*(cur + BASIC_BLOCK_SIZE)->var.val.var_ptr); 
		(cur + BASIC_BLOCK_SIZE)->var.val.var_ptr = (var_t*)_calloc( BASIC_BLOCK_SIZE + 1 , sizeof(var_t)); 
		counter = i / BASIC_BLOCK_SIZE;
		cur = table->var;
	}
	while(counter--) cur = &(*(cur + BASIC_BLOCK_SIZE)->var.val.var_ptr); 
	cur += i % BASIC_BLOCK_SIZE;
	memcpy( cur , var , sizeof(var_t) );
	cur->name = strdup(var->name);

	/*
	cur->name = var->name;
	cur->var.type = var->var.type;
	if(var->var.type == INT)
		table->var[i].var.val.int_t = var->var.val.int_t;
	else if(var->var.type == DOUBLE)
		table->var[i].var.val.double_t = var->var.val.double_t;
	else if(var->var.type == STRING)	
		table->var[i].var.val.char_t = var->var.val.char_t;
	else if(var->var.type == TABLE)
		table->var[i].var.val.table = var->var.val.table;
	*/
	return i;
}


int add_to_element_table(element_table_t * table , element_var_t * var )
{
	element_var_t * cur = table->var;
	int i = table->count++;
	int counter = i / BASIC_BLOCK_SIZE;
	if( i && !(i % BASIC_BLOCK_SIZE) ) 
	{
		while(--counter) cur = &(*(cur + BASIC_BLOCK_SIZE)->val.evar_ptr); 
		(cur + BASIC_BLOCK_SIZE)->val.evar_ptr = (element_var_t*)_calloc( BASIC_BLOCK_SIZE + 1 , sizeof(element_var_t)); 
		counter = i / BASIC_BLOCK_SIZE;
		cur = table->var;
	}
	while(counter--) cur = &(*(cur + BASIC_BLOCK_SIZE)->val.evar_ptr); 
	cur += i % BASIC_BLOCK_SIZE;
	memcpy( cur , var , sizeof(element_var_t) );
	
	/*
	cur->name = var->name;
	cur->var.type = var->var.type;
	if(var->var.type == INT)
		table->var[i].var.val.int_t = var->var.val.int_t;
	else if(var->var.type == DOUBLE)
		table->var[i].var.val.double_t = var->var.val.double_t;
	else if(var->var.type == STRING)	
		table->var[i].var.val.char_t = var->var.val.char_t;
	else if(var->var.type == TABLE)
		table->var[i].var.val.table = var->var.val.table;
	*/
	return i;
}


int get_index_in_table(table_t * table , char * name)
{
	int i;
	var_t * cur;
	for(i=0;i<table->count;i++)
	{
		cur = table->var;
		int counter = i / BASIC_BLOCK_SIZE;
		while(counter--) cur = &(*(cur + BASIC_BLOCK_SIZE)->var.val.var_ptr); 
		cur += i % BASIC_BLOCK_SIZE;
		if( !strcmp( (cur->name)?(cur->name):"\0" , name) && strlen(cur->name) == strlen(name)) 
			return i;
	}
	return -1;
	fprintf(stderr, "Error : Name %s is not defined\n", name );
}

var_t get_from_table(table_t * table , char * name)
{
	int i;
	var_t * cur;
	for(i=0;i<table->count;i++)
	{
		cur = table->var;
		int counter = i / BASIC_BLOCK_SIZE;
		while(counter--) cur = &(*(cur + BASIC_BLOCK_SIZE)->var.val.var_ptr); 
		cur += i % BASIC_BLOCK_SIZE;
		if( !strcmp( (cur->name)?(cur->name):"\0" , name) && strlen(cur->name) == strlen(name)) 
			return *cur;
	}
	fprintf(stderr, "Error : Name %s is not defined\n", name );
	chalk_exit();
}

void dump_table(table_t * table)
{
	printf("Dump of table %s\n", table->name );
	printf("COUNT = %d\n" , table->count );
	printf("NAME\tTYPE\tVALUE\n");
	int i;
	for(i=0;i<table->count;i++)
	{
		var_t * cur = table->var;
		int counter = i / BASIC_BLOCK_SIZE;
		while(counter--) cur = &(*(cur + 32)->var.val.var_ptr); 
		cur += i % BASIC_BLOCK_SIZE;
		printf("%d:\t" , i );
		if(cur->var.type == INT)
			printf("%s\tINT\t%d\n", cur->name , cur->var.val.int_t );
		else if (table->var[i].var.type == DOUBLE)
			printf("%s\tDOUBLE\t%f\n", table->var[i].name , table->var[i].var.val.double_t );
		else if (table->var[i].var.type == STRING)
			printf("%s\tSTRING\t%s\n", table->var[i].name , table->var[i].var.val.char_t  );
		else if (table->var[i].var.type == TABLE)
			printf("%s\tTABLE\t@%ld\n", table->var[i].name , (long)(table->var[i].var.val.table) );
		else 
			printf("%s\t(%d)\t@%ld\n" , cur->name , cur->var.type , (long)(&cur->var.val) );
	}
}

void dump_element_table(element_table_t * table)
{
	printf("Dump of table %s\n", table->name );
	printf("N\tTYPE\tVALUE\n");
	int i;
	for(i=0;i<table->count;i++)
	{
		if(table->var[i].type == INT)
			printf("%d\tINT\t%d\n", i , table->var[i].val.int_t );
		else if (table->var[i].type == DOUBLE)
			printf("%d\tDOUBLE\t%f\n", i , table->var[i].val.double_t );
		else if (table->var[i].type == STRING)
		{
			printf("%d\tSTRING\t%s\n", i , table->var[i].val.char_t  );
		}
		else if (table->var[i].type == TABLE)
			printf("%d\tTABLE\t@%ld\n", i , (long)(table->var[i].val.table) );
	}
}

#endif

/*
int main(int argc , char *argv[])
{
	table_t * Global = init_table("Global");
	Global->var = malloc(sizeof(var_t));
	
	var_t var1 = { .name = "h1" , .var.type = STRING , .var.val.char_t = "My " };
	var_t var2 = { .name = "h2" , .var.type = STRING , .var.val.char_t = "String"};




	//char defstr[]  = "A String";

	/////////////////////////////
	
	int len1 = strlen(var1.var.val.char_t);
	int len2 = strlen(var2.var.val.char_t);
	char tmp[len1+len2];
	int i,j=0;
	for(i=0;i<len1;i++)
		tmp[j++] = var1.var.val.char_t[i];
	for(i=0;i<len2;i++)
		tmp[j++] = var2.var.val.char_t[i];
	tmp[j] = '\0';
	
	////////////////////////////

	var_t var3 = { .name = "h1+h2" , .var.type = STRING , .var.val.char_t = add_string(var1 , var2) };
	
	
	
	fprintf(stderr, "%s\n", var3.var.val.char_t );

	add_to_table(Global , &var1);
	add_to_table(Global , &var2);
	add_to_table(Global , &var3);

	//fprintf(stderr, "%s\n", Global->var[2].var.val.char_t );
	
	int k;
	for(k=0;k<5;k++)
		fprintf(stderr , "%d\t%c\n" , Global->var[2].var.val.char_t[k] , Global->var[2].var.val.char_t[k] );

	//fprintf(stderr, "%s\n", get_from_table(Global , "h1+h2").var.val.char_t );
	
	dump_table(Global);
	
	return 0;
}
*/