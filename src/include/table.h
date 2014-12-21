#include "var.h"
#include "arithmetics.h"
#include <string.h>

#ifndef TABLE_C
#define TABLE_C

table_t * init_table(char * name)
{
	table_t * tmp = malloc(sizeof(table_t));
	tmp->name = name;
	tmp->count = 0;
	return tmp;
}

int table_index(table_t * table , int index ,  element_var_t * var)
{
	table->var[index].var.type = var->type;
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

int add_to_table(table_t * table , var_t * var )
{
	//printf("Adding to table %s , name = %s\n" , table->name , var->name );
	/*
	 * Checking if the table already has this member
	 */
	int i;
	if(var->name)
	{
		for(i=0;i<table->count;i++)
		{
			if(!strcmp( (table->var[i].name)?(table->var[i].name):"\0" , var->name))
			{
				//fprintf(stderr, "Error : Variable %s is already defined\n", var->name );
				//exit(0);
				break;
			}
		}
	}
	else
	{
		i = table->count;
	}
	if(i == table->count)table->count++;
	table->var[i].name = var->name;
	table->var[i].var.type = var->var.type;
	if(var->var.type == INT)
		table->var[i].var.val.int_t = var->var.val.int_t;
	else if(var->var.type == DOUBLE)
		table->var[i].var.val.double_t = var->var.val.double_t;
	else if(var->var.type == STRING)	
		table->var[i].var.val.char_t = var->var.val.char_t;
	else if(var->var.type == TABLE)
		table->var[i].var.val.table = var->var.val.table;
	return i;
}

int get_index_in_table(table_t * table , char * name)
{
	int i;
	for(i=0;i<table->count;i++)
	{
		if(!strcmp((table->var[i].name)?(table->var[i].name):"\0" , name))
			return i;
	}
	return -1;
	fprintf(stderr, "Error : Name %s is not defined\n", name );
}

var_t get_from_table(table_t * table , char * name)
{
	int i;
	for(i=0;i<table->count;i++)
	{
		if(!strcmp((table->var[i].name)?(table->var[i].name):"\0" , name))
			return table->var[i];
	}
	fprintf(stderr, "Error : Name %s is not defined\n", name );
	exit(0);
}

void dump_table(table_t * table)
{
	printf("Dump of table %s\n", table->name );
	printf("NAME\tTYPE\tVALUE\n");
	int i;
	printf("COUNT = %d\n" , table->count );
	for(i=0;i<table->count;i++)
	{
		if(table->var[i].var.type == INT)
			printf("%s\tINT\t%d\n", table->var[i].name , table->var[i].var.val.int_t );
		else if (table->var[i].var.type == DOUBLE)
			printf("%s\tDOUBLE\t%f\n", table->var[i].name , table->var[i].var.val.double_t );
		else if (table->var[i].var.type == STRING)
			printf("%s\tSTRING\t%s\n", table->var[i].name , table->var[i].var.val.char_t  );
		else if (table->var[i].var.type == TABLE)
			printf("%s\tTABLE\t@%ld\n", table->var[i].name , (long)(table->var[i].var.val.table) );
		else 
			printf("%s\t(%d)\t@%ld\n", table->var[i].name , table->var[i].var.type , (long)(&table->var[i].var.val) );
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