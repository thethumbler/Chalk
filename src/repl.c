/* Read evaluate print loop .. interactive shell */

int eval(char * str)
{
	src = str;
    parse();
    launch(chunk_buf , Global->count , Strings , Global);
}

int repl(void)
{
	init_int();
	while(1)
	{
		char * str = calloc(MAX_READ_BUFFER , sizeof(char));
		fgets(str , MAX_READ_BUFFER , stdin );
		char * dynamic_str = calloc(strlen(str) + 1 , sizeof(char));
		memcpy(dynamic_str , str , strlen(str) );
		free(str);
		eval("print(1)\nprint(2)");
	}
}
