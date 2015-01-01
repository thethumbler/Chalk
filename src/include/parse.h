int is_uminus = 0;

char * input_file_name;

void parse_error()
{
	fprintf(stderr, "error in file %s\n" , input_file_name );
	if(in_func)
	{
		fprintf( stderr , "in function %s , " , func_list_name[cur_func_index-1] );

	} else
	{
		fprintf( stderr , "in main chunk , ");		
	}
	fprintf( stderr , "line %d :\n" , pos_y );
	while(src[cur_line_start_pos] != '\n' && src[cur_line_start_pos] != '\0')
        fprintf( stderr , "%c" , src[cur_line_start_pos++] );
    fprintf(stderr, "\n");
}

char * tokentostring(tok token)
{
	char * ret;
	switch(token.token)
	{
		case INT_VAL: 	ret = calloc(strlen(lextext) + strlen("number "),sizeof(char));
					  	strcpy(ret , "number ");
					  	return strcat( ret , lextext );
		case STRING_VAL: return "string literal";
		case 	'\n': 	return "end of line";
		default   	: 	if(token.token <= 255) 
						{
							ret = calloc(2 , 1);
							ret[0] = (char)token.token;
							ret[1] = '\0';
							return ret;
						}
					  	return token2string[token.token - 256];
	}
}

int parse_call_parameters();