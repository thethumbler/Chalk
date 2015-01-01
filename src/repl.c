/* Read evaluate print loop .. interactive shell */

#define MAX_GLOBALS 4096

void init_repl()
{
	L_Head = malloc(sizeof(Local_tables_t));
	RT_Global = malloc(sizeof(element_table_t));
	RT_Global->name = "RT_Global";
	RT_Global->count = MAX_GLOBALS;
	RT_Global->var = calloc( MAX_GLOBALS , sizeof(element_var_t));
}

int eval(char * str)
{
	src = str;
	pos = 0;
    parse();
    _asm(OP_HALT , NULL);
    running = 1;
    pc = 0;
    exec(chunk_buf , Strings);
}

int repl(void)
{
	init_repl();
	init_int();
	while(1)
	{
		empty(chunk_buf , chunk_buf_pos);
		chunk_buf_pos = 0;
		printf("> ");
		char * str = calloc(MAX_READ_BUFFER , sizeof(char));
		fgets(str , MAX_READ_BUFFER , stdin );
		char * dynamic_str = calloc(strlen(str) + 1 , sizeof(char));
		memcpy(dynamic_str , str , strlen(str) );
		free(str);
		eval(dynamic_str);
		printf("\n");
		//eval("x=1\nprint(x)");
	}
	chalk_exit();
}
