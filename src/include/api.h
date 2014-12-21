#include <string.h>
#include <math.h>

#ifndef API_H
#define API_H
void api_print(void)
{
	element_var_t var = *op_pop();
	switch(var.type)
	{
		case 	INT : fprintf(stderr , "%d" , var.val.int_t ); break;
		case DOUBLE : fprintf(stderr , "%.15lf" , var.val.double_t ); break;
		case STRING : fprintf(stderr , "%s" , var.val.char_t ); break;
	}
}

/* 4 Kilobytes buffer */
#define MAX_READ_BUFFER 4096	

void api_read(void)
{
	char * str = calloc(MAX_READ_BUFFER , sizeof(char));
	fgets(str , MAX_READ_BUFFER , stdin );
	char * dynamic_str = calloc(strlen(str) , sizeof(char));
	memcpy(dynamic_str , str , strlen(str) - 1 );
	free(str);
	op_push(*new_var( STRING , (var_union)(dynamic_str)));
}

void api_open(void)
{
	char * filename = op_pop()->val.char_t; 
	char * str = open_file(filename);
	op_push(*new_var( STRING , (var_union)(str)));
}

void api_write(void)
{
	char * str = op_pop()->val.char_t;
	char * filename = op_pop()->val.char_t;
	FILE * out = fopen( filename , "w" );
	fwrite( str , strlen(str) , 1 , out);
	fclose( out );
}

void api_tonumber(void)
{
	op_push(*new_var( INT , (var_union)(atoi((*op_pop()).val.char_t))));
}

void api_toreal(void)
{
	op_push(*new_var( INT , (var_union)(atof((*op_pop()).val.char_t))));
}

void api_sin(void)
{
	element_var_t tmp = *op_pop();
	switch(tmp.type)
	{
		case   	INT: op_push(*new_var( DOUBLE , (var_union)(sin(tmp.val.int_t)))); break;
		case DOUBLE: op_push(*new_var( DOUBLE , (var_union)(sin(tmp.val.double_t)))); break;
	}
}

void api_cos(void)
{
	element_var_t tmp = *op_pop();
	switch(tmp.type)
	{
		case   	INT: op_push(*new_var( DOUBLE , (var_union)(cos(tmp.val.int_t))));
		case DOUBLE: op_push(*new_var( DOUBLE , (var_union)(cos(tmp.val.double_t))));
	}
}

void api_tan(void)
{
	element_var_t tmp = *op_pop();
	switch(tmp.type)
	{
		case   	INT: op_push(*new_var( DOUBLE , (var_union)(tan(tmp.val.int_t))));
		case DOUBLE: op_push(*new_var( DOUBLE , (var_union)(tan(tmp.val.double_t))));
	}
}

void api_abs(void)
{
	element_var_t tmp = *op_pop();
	switch(tmp.type)
	{
		case   	INT: op_push(*new_var( INT , (var_union)(abs(tmp.val.int_t))));
		case DOUBLE: op_push(*new_var( DOUBLE , (var_union)(fabs(tmp.val.double_t))));
	}
}

void api_sqrt(void)
{
	element_var_t tmp = *op_pop();
	switch(tmp.type)
	{
		case   	INT: op_push(*new_var( INT , (var_union)(sqrt(tmp.val.int_t))));
		case DOUBLE: op_push(*new_var( DOUBLE , (var_union)(sqrt(tmp.val.double_t))));
	}	
}

char * api_func_list[] = {
	"print" , "read" , "open" , "write" , "tonumber" , "toreal" , "sin" , "cos" , "tan" , "abs" , ";"
};

typedef void (*api_func)(void);

api_func api[] =
{
	NULL , 
	api_print ,
	api_read ,
	api_open ,
	api_write ,
	api_tonumber ,
	api_toreal ,
	api_sin ,
	api_cos ,
	api_tan ,
	api_abs
};

int is_api_func(char * id)
{
	int i = 0;
	while(strcmp(id,api_func_list[i]))
		if(api_func_list[i++][0] == ';') 
			return 0;
	return ++i;
}
#endif
