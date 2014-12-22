/* Read evaluate print loop .. interactive shell */

int eval(char * str)
{
	src = str;
	pos = 0;
    parse();
    _asm(OP_HALT , NULL);
    running = 1;
    pc = 0;
    launch(chunk_buf , Global->count , Strings , Global);
}

int repl(void)
{
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
		//eval("print(1)\nprint(2)");
	}
}
