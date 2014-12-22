#include "lexer.c"
#include "include/parse.h"
int parse_func()
{
    int parm_count = 0;
    if(token.nxt.token != ID)    
    {
        parse_error();
        fprintf(stderr , "Error : expected function name after token 'function'\n");
        exit(0);
    }
    lex();
    char * id = token.cur.text;
    func_list_loc[cur_func_index] = inst_count;
    func_list_name[cur_func_index++] = strdup(lextext);
    new_local_table();
    //add_to_table(Global , new_var_t(func_list_name[cur_func_index-1] , new_var(TABLE , (var_union)Local)));
    //dump_table(Global);
    int cur_pos = pos;
    parm_count = 0;
    if(token.nxt.token != '(')
    {
        parse_error();
        fprintf(stderr , "Error : error expected '(' before %s\n" , tokentostring(token.nxt));
        exit(0);
    }
    lex();
    while(token.cur.token != ')') 
    {
        lex();
        switch(token.cur.token)
        {
            case  ID: 
                parm_count++; 
                if(debug)fprintf( stderr , "SLV\t%s\n" , token.cur.text );
                _asm(OP_SLV , new_var( STRING , (var_union)(token.cur.text)));
                continue;
            case ',':
                continue;
            case ')': 
                continue;
        }
        parse_error();
        fprintf(stderr , "Error : error expected ')' before %s\n" , tokentostring(token.cur));
        exit(0);
    }
    lex();
    if(debug)fprintf( stderr , "*%s:%d\n" , id , parm_count);
    return 0;
}

int parse_term()    /* Matches a mathematical term i.e. it terminates with '+' or '-' */
{
    lex();
    //printf("tok = [%d] %s\n" , token.cur.token , tokentostring(token.cur) );
    switch(token.cur.token)
    {
        case INT_VAL:
            if(debug)fprintf( stderr , "PUSH\t%d\n" , atoi(lextext));
            _asm(OP_PUSH , new_var(INT , (var_union)atoi(lextext)));
            if(is_in(token.nxt.token,"*/")) parse_term();
            return 0;

        case DOUBLE_VAL:
            if(debug)fprintf( stderr , "PUSH\t%f\n" , atof(lextext));
            _asm(OP_PUSH , new_var( DOUBLE , (var_union)atof(lextext)));
            return 0;

        case STRING_VAL:
            if(debug){ fprintf( stderr , "LSTR\t\""); print_lstr(lextext); fprintf( stderr , "\"\n"); } 
            _asm(OP_LSTR , new_var( STRING , (var_union)strdup(lextext)));
            if(is_in(token.nxt.token , "*/-"))
            {
                parse_error();
                fprintf(stderr, "error trying to perform mathematical operation on string literal\n");
                exit(0);
            }
            return 0;

        case    ID:    
            pre_id = id;
            id = strdup(token.cur.text);
            if ( token.nxt.token == '(' ) //Function call
            {
                if(is_api_func(id))
                {
                    int api_f = is_api_func(id);
                    parse_call_parameters();
                    if(debug)fprintf( stderr , "CAPI\t%d\n" , api_f );
                    _asm(OP_CAPI , new_var( INT , (var_union)api_f));
                    return 1;
                }
                if(get_func_index(id) == -1)
                {
                    parse_error();
                    fprintf( stderr , "function %s is not defined\n" , id );
                    exit(0);
                }
                int fun_loc = func_list_loc[get_func_index(id)];
                parse_call_parameters();    /* parse_expr(); */
                if(debug)fprintf( stderr , "CALL\t%d\n" , fun_loc - 2);
                _asm(OP_CALL , new_var( INT , (var_union)(fun_loc - 2)));
                return 1;
            }
            else if(token.nxt.token == '[') //requesting table element
            {
                lex();
                char * element_name;
                if(token.nxt.token == STRING_VAL)
                {
                    lex();
                    element_name = strdup(lextext);
                    if (in_func) 
                    {
                    
                        if(get_index_in_table(Local , id) == -1 && get_index_in_table(Global , id) == -1)
                        {
                            parse_error();
                            fprintf( stderr , "name %s is not declared in this function or in the main chunk\n" , id );
                            exit(0);
                        }
                        if(debug)fprintf( stderr , "LLV\t%s\n" , id );
                        _asm(OP_LLV , new_var( STRING , (var_union)strdup(id)));
                    }
                    else
                    {
                        if(get_index_in_table(Global , id) == -1)
                        {   
                            parse_error();
                            fprintf( stderr , "name %s is not declared\n" , id );
                            exit(0);
                        }
                        if(debug)fprintf( stderr , "LGV\t%s\n" , id );
                        _asm(OP_LGV , new_var( STRING , (var_union)id));
                    }
                    id = pre_id;
                    if(debug)fprintf( stderr , "LSTR\t%s\n" , element_name );
                    _asm(OP_LSTR , new_var( STRING , (var_union)element_name));
                    if(debug)fprintf( stderr , "LTE\n");
                    _asm(OP_LTE , NULL );
                    lex();
                    return 0;
                }
                parse_error();
                fprintf(stderr, "Tables can be indexed only using a string\n");
                exit(0);
                //parse_expr();
            }
            else     //Normal identifier
            { 
                if (in_func) 
                {
                    
                    if(get_index_in_table(Local , id) == -1 && get_index_in_table(Global , id) == -1)
                    {
                        parse_error();
                        fprintf( stderr , "name %s is not declared in this function or in the main chunk\n" , id );
                        exit(0);
                    }
                    if(debug)fprintf( stderr , "LLV\t%s\n" , id );
                    _asm(OP_LLV , new_var( STRING , (var_union)strdup(id)));
                }
                else 
                {
                    if(get_index_in_table(Global , id) == -1)
                    {
                        parse_error();
                        fprintf( stderr , "name %s is not declared\n" , id );
                        exit(0);
                    }
                    if(debug)fprintf( stderr , "LGV\t%s\n" , id );
                    _asm(OP_LGV , new_var( STRING , (var_union)id));
                }
                id = pre_id; 
                if(is_in(token.nxt.token,"*/")) parse_term();
                return 1;
            }

        case '*':
            parse_term(); 
            if(debug)fprintf( stderr , "MUL\n");    
            _asm(OP_MUL , NULL);
            return 1;

        case '/':
            parse_term(); 
            if(debug)fprintf( stderr , "DIV\n");    
            _asm(OP_DIV , NULL);
            return 1;

        case '(':
            if(token.nxt.token == ')')  /* Empty parenthesis */
                return 0;
            parse_expr(); 
            if(token.nxt.token != ')')
            {
                parse_error();
                fprintf(stderr, "expected ')' before %s\n" , tokentostring(token.nxt) );
                exit(0);
            }
            lex();
            if(is_in(token.nxt.token,"*/")) parse_term();
            return 0;

        case '+':
        case '-':
        case '>':
        case '<':
        case  LE:
        case  GE:
        case  EQ:
        case  NE:
        case ',': return 0;
    }

    parse_error();
    printf("expected expression before %s\n" , tokentostring(token.cur) );
    exit(0);
    
}

int parse_expr()
{
    parse_term();

parse_expr_only:
    //printf("tok = [%d] %s\n" , token.nxt.token , tokentostring(token.nxt) );
    switch(token.nxt.token)
    {
        case '+':
            lex();
            parse_term();
            if(debug)fprintf( stderr , "ADD\n");
            _asm(OP_ADD , NULL);
            if(is_in(token.nxt.token , "+-")) goto parse_expr_only;
            return 0;

        case '-':
            lex();
            parse_term();
            if(debug)fprintf( stderr , "SUB\n");
            _asm(OP_SUB , NULL);
            if(is_in(token.nxt.token , "+-")) goto parse_expr_only;
            return 0;

        case '>':    
            lex();
            parse_expr();
            if(debug)fprintf( stderr , "TGT\n");    
            _asm(OP_TGT , NULL);
            return 0;

        case '<':
            lex();
            parse_expr();
            if(debug)fprintf( stderr , "TLT\n");    
            _asm(OP_TLT , NULL);
            return 0;

        case LE:
            lex();
            parse_expr();
            if(debug)fprintf( stderr , "TLE\n");    
            _asm(OP_TLE , NULL);
            return 0;

        case GE:
            lex();
            lex();
            parse_expr();
            if(debug)fprintf( stderr , "TGE\n");    
            _asm(OP_TGE , NULL);
            return 0;

        case EQ:
            lex();
            parse_expr();
            if(debug)fprintf( stderr , "TEQ\n");
            _asm(OP_TEQ , NULL);
            return 0;

        case NE:
            lex();
            lex();
            parse_expr();
            if(debug)fprintf( stderr , "TNE\n");    
            _asm(OP_TNE , NULL);
            return 0;
        /*
        case ',':
            lex();
            parse_expr();
            return 0;
        */
    }

    /*
    if(token.nxt.token == '+')
    {
        lex();
        parse_term();
        if(debug)fprintf( stderr , "ADD\n");
        _asm(OP_ADD , NULL);
        return 0;
    }
    else if(token.nxt.token == '-')
    {
        lex();
        parse_term();
        if(debug)fprintf( stderr , "SUB\n");
        _asm(OP_SUB , NULL);    
        if(is_in(token.nxt.token , "+-")) goto parse_expr_only;  
        else if(is_in(token.nxt.token , "*")) parse_expr();
        return 0;
    }
    */
}

int parse_call_parameters()
{
    lex();
    int count = 0;
    if(token.nxt.token == ')')  //Zero-Parameters function call
    {
        lex();
        return 0;
    }
parse_param:
    parse_expr();
    //printf("tok = [%d] %s\n" , token.nxt.token , tokentostring(token.nxt) );
    if(token.nxt.token == ')')  //Zero-Parameters function call
    {
        lex();
        return 0;
    }
    if(token.nxt.token == ',')
    {
        count++;
        lex();
        goto parse_param;
    }
    return count;
}

int parse_table_elements()
{
    if(token.nxt.token == '}')  return 0;   /* Empty table */
    while(token.nxt.token != '}')
    {
        if(token.nxt.token == '\n') 
        {
            lex();
            continue;
        }
        if(token.nxt.token == STRING_VAL)
        {
            char * element_name = strdup(lextext);
            lex();
            if(token.nxt.token == ASSIGN)
            {
                lex();
                parse_expr();
                while(token.nxt.token == '\n')lex();
                if(token.nxt.token == '}')
                {
                    if(debug){ fprintf( stderr , "LSTR\t\""); print_lstr(element_name); fprintf( stderr , "\"\n"); } 
                    _asm(OP_LSTR , new_var( STRING , (var_union)strdup(element_name)));
                    if(debug)fprintf( stderr , "STE\n");    
                    _asm(OP_STE , new_var( NULL_T , (var_union)(0)));
                    return 0;
                }
                else if(token.nxt.token == ',')
                {
                    lex();
                    if(debug){ fprintf( stderr , "LSTR\t\""); print_lstr(element_name); fprintf( stderr , "\"\n"); } 
                    _asm(OP_LSTR , new_var( STRING , (var_union)strdup(element_name)));
                    if(debug)fprintf( stderr , "STE\n");    
                    _asm(OP_STE , new_var( NULL_T , (var_union)(0)));
                    continue;
                }
            }
            if(debug){ fprintf( stderr , "LSTR\t\""); print_lstr(element_name); fprintf( stderr , "\"\n"); } 
            _asm(OP_LSTR , new_var( STRING , (var_union)strdup(element_name)));
            if(debug)fprintf( stderr , "STE\n");    
            _asm(OP_STE , new_var( NULL_T , (var_union)(0)));
        }
    }
    return 0;
    _error:
    parse_error();
    printf("Error declaring table\n");
    exit(0);
}

int parse_statment()
{
    //if(!(pos<strlen(src))) return 0;
    lex();
    //printf("TOK = %s\n" , tokentostring(token.cur) );
    switch(token.cur.token)
    {
        case    LEXEOF:    
            return 0;

        case    '\n':    
            return '\n';

        case    ID:    
            pre_id = id;
            id = strdup(lextext);
            if(token.nxt.token == ASSIGN)  
            { 
                lex();
                char * assign_id = id;
                if(token.nxt.token == '{')  //Table 
                {
                    lex();
                    if(debug)fprintf( stderr , "CT\t%s\n" , assign_id ); 
                    _asm(OP_CT , NULL);
                    parse_table_elements();
                    if(token.nxt.token != '}')
                    {
                        parse_error();
                        fprintf(stderr, "error expected '}' before %s\n" , tokentostring(token.nxt) );
                        exit(0);
                    }
                    lex();
                    if(debug)fprintf( stderr , "LT\t%s\n" , assign_id ); 
                    _asm(OP_LT , NULL);
                    goto _jmp_out;
                }
                parse_expr();
                _jmp_out:
                if(in_func)
                {
                    if(debug)fprintf( stderr , "SLV\t%s\n" , assign_id );
                    _asm(OP_SLV , new_var( STRING , (var_union)assign_id));
                    return 1;
                }
                if(debug)fprintf( stderr , "SGV\t%s\n" , assign_id ); 
                _asm(OP_SGV , new_var( STRING , (var_union)assign_id));
                return 1; 
            }
            else if (token.nxt.token == '(' )
            {
                //lex();
                //printf("%d\n" , token.cur.token );
                if(is_api_func(id))
                {
                    int api_f = is_api_func(id);
                    parse_call_parameters();    /* parse_expr(); */
                    if(debug)fprintf( stderr , "CAPI\t%d\n" , api_f );
                    _asm(OP_CAPI , new_var( INT , (var_union)api_f));
                    return 1;
                }
                if(get_func_index(id) == -1)
                {
                    parse_error();
                    fprintf( stderr , "function %s is not defined\n" , id );
                    exit(0);
                }
                int fun_loc = func_list_loc[get_func_index(id)];
                parse_call_parameters();    /* parse_expr(); */
                if(debug)fprintf( stderr , "CALL\t%d\n" , fun_loc - 2);
                _asm(OP_CALL , new_var( INT , (var_union)(fun_loc - 2)));
                return 1;
            }

        case    IF:    
            //level = max_lvl+1; 
            in_cond = 1;
            parse_expr();
            if(token.nxt.token != THEN)
            {
                parse_error();
                fprintf( stderr , "expected 'then' before %s\n" , tokentostring(token.nxt) );
                exit(0);
            } 
            lex();
            in_cond = 0;
            int dummy = chunk_buf_pos;
            if(debug)fprintf( stderr , "JZ\t%d\n" , 0 );  
            _asm(OP_JZ , new_var( INT , (var_union)(0) ));
            //int cur_pos = pos;
            //int tmp_counting = counting , tmp_debug = debug; debug = 0;
            //counting = 1;
            //tmp_inst_count = inst_count;
            while(parse_statment());
            if(token.cur.token != END)
            {
                parse_error();
                fprintf( stderr , "expected 'end' before %s\n" , tokentostring(token.cur) );
                exit(0);
            } 
            //counting = tmp_counting;
            //debug = tmp_debug;
            int if_block_size = (chunk_buf_pos - dummy)/10 - 1;
            memcpy( &chunk_buf[dummy+2] , &if_block_size , sizeof(int));
            //pos = cur_pos;
            //while(parse(src) != END);
            //inst_count -= if_block_size;
            return 1;
        case    WHILE:    
            //level = max_lvl+1; 
            //if(debug)fprintf( stderr , "w%d:\n", wh_lvl++ ); 
            in_cond = 1;
            dummy = chunk_buf_pos;
            parse_expr(); 
            if(token.nxt.token != DO)
            {
                parse_error();
                fprintf( stderr , "expected 'do' before %s\n" , tokentostring(token.nxt) );
                exit(0);
            } 
            lex();
            int tdum = chunk_buf_pos;
            if(debug)fprintf( stderr , "JZ\t%d\n" , 0 );  
            _asm(OP_JZ , new_var( INT , (var_union)(0) ));
            in_cond = 0;
            //tmp_inst_count = inst_count;
            //counting = 1; int tmp_debug = debug; debug = 0;
            //while(parse(src) != END); 
            //counting = 0; debug = tmp_debug;
            //inst_count = tmp_inst_count + 1
            //pos = cur_pos;
            while(parse_statment()); 
            if(token.cur.token != END)
            {
                parse_error();
                fprintf( stderr , "expected 'end' before %s\n" , tokentostring(token.cur) );
                exit(0);
            }
            int while_block_size = (chunk_buf_pos - dummy)/10 + 1;
            if(debug)fprintf( stderr , "JMP\t%d\n" , -while_block_size);
            _asm(OP_JMP , new_var(INT , (var_union)(-while_block_size))); 
            int while_size = (chunk_buf_pos - tdum)/10 - 1;
            memcpy( &chunk_buf[tdum+2] , &while_size , sizeof(int) ); 
            //if(debug)fprintf( stderr , "%d:\n" , --level); 
            return 1;
        case    FUNC:    
            in_func = 1;
            dummy = chunk_buf_pos;
            /*cur_pos = pos;
            counting = 1; tmp_debug = debug; debug = 0;
            parse_func(src); 
            while(parse(src) != END); 
            counting = 0;
            int jmp_v = inst_count - dummy;
            inst_count = dummy;
            debug = tmp_debug;
            */
            if(debug)fprintf( stderr , "JMP\t%d\n" , 0);
            _asm(OP_JMP , new_var(INT , (var_union)(0)));
            if(debug)fprintf( stderr , "LLT\t%d\n" , 0);
            _asm(OP_LLT , new_var(INT , (var_union)(0)));
            //pos = cur_pos;
            parse_func(); 
            while(parse_statment());
            if(token.cur.token != END)
            {
                parse_error();
                fprintf( stderr , "expected 'end' before %s\n" , tokentostring(token.cur) );
                exit(0);
            }
            int func_size = (chunk_buf_pos - dummy)/10;
            memcpy( &chunk_buf[dummy+2] , &func_size , sizeof(int) );
            memcpy( &chunk_buf[dummy+12] , &(Local->count) , sizeof(int) );
            if(debug)fprintf( stderr , "RET\n"); 
            _asm(OP_RET , NULL);
            in_func = 0;
            return 1;

        case RETURN:    
            parse_expr();
            if(debug)fprintf( stderr , "RET\n");
            _asm(OP_RET , NULL);
            return 1;
        case END:
            return 0;

    }
    parse_error();
    fprintf( stderr , "expected statment before %s token\n" , tokentostring(token.cur) );
    exit(0);
}

int parse_prgrm()
{
    while(parse_statment());
    //while(parse_term() != LEXEOF);
    //parse_expr();
}

int parse_tmp()
{
	if(!(pos<strlen(src))) return 0;
	int a = lex(src) , b;
    switch(a)
    {
    	case 	 LEXEOF:	return 0;
    	case 	   '\n':	return '\n';
        case        ',':    return ',';

      	case 	INT_VAL:	if(debug)fprintf( stderr , "PUSH\t%d\n" , atoi(lextext));
                            _asm(OP_PUSH , new_var(INT , (var_union)atoi(lextext)));
                                return 1;

        case DOUBLE_VAL:    if(debug)fprintf( stderr , "PUSH\t%f\n" , atof(lextext));
                            _asm(OP_PUSH , new_var( DOUBLE , (var_union)atof(lextext)));
                                return 1;

       	case 		'+':	while(parse(src) > 1); 
                            if(debug)fprintf( stderr , "ADD\n");
                            _asm(OP_ADD , NULL);    
                            return 2;

       	case 		'-':	while(parse(src) > 1); 
                            if(debug)fprintf( stderr , "SUB\n");	
                            _asm(OP_SUB , NULL);
                            return 2;

       	case 		'*':	while(parse(src) > 2); 
                            if(debug)fprintf( stderr , "MUL\n");	
                            _asm(OP_MUL , NULL);
                            return 3;

       	case 		'/':	while(parse(src) > 2); 
                            if(debug)fprintf( stderr , "DIV\n");	
                            _asm(OP_DIV , NULL);
                            return 3;
        case        '^':    while(parse(src) > 3); 
                            if(debug)fprintf( stderr , "POW\n");    
                            _asm(OP_POW , NULL);
                            return 3;

        case        '>':    while(parse(src) > 1); 
                            if(debug)fprintf( stderr , "TGT\n");    
                            _asm(OP_TGT , NULL);
                            return 1;

        case        '<':    while(parse(src) > 1); 
                            if(debug)fprintf( stderr , "TLT\n");    
                            _asm(OP_TLT , NULL);
                            return 2;

        case         LE:    while(parse(src)); 
                            if(debug)fprintf( stderr , "TLE\n");    
                            _asm(OP_TLE , NULL);
                            return 2;

        case         GE:    while(parse(src) > 1); 
                            if(debug)fprintf( stderr , "TGE\n");    
                            _asm(OP_TGE , NULL);
                            return 2;

        case         EQ:    while(parse(src) > 1); 
                            if(debug)fprintf( stderr , "TEQ\n");
                            _asm(OP_TEQ , NULL);
                            return 2;

        case         NE:    while(parse(src) > 1); 
                            if(debug)fprintf( stderr , "TNE\n");    
                            _asm(OP_TNE , NULL);
                            return 2;

       	case 		'(':	while(parse(src) != ')');	return 0;
       	case 		')':	return ')';

		case 		 ID:	
							pre_id = id;
							id = strdup(lextext);
							if(lex(src) == ASSIGN)	
                            { 
                                char * assign_id = id;
                                while(parse() != 0);
                                if(in_func)
                                {
                                    if(debug)fprintf( stderr , "SLV\t%s\n" , assign_id );
                                    _asm(OP_SLV , new_var( STRING , (var_union)assign_id));
                                    return 1;
                                }
                                if(debug)fprintf( stderr , "SGV\t%s\n" , assign_id ); 
                                _asm(OP_SGV , new_var( STRING , (var_union)assign_id));
                                return 1; 
                            }
							else if ( pos=start_pos , lex(src) == '(' )
                            {
                                if(is_api_func(id))
                                {
                                    int api_f = is_api_func(id);
                                    while(parse(src) != ')');
                                    if(debug)fprintf( stderr , "CAPI\t%d\n" , api_f );
                                    _asm(OP_CAPI , new_var( INT , (var_union)api_f));
                                    return 1;
                                }
                                if(get_func_index(id) == -1)
                                {
                                        parse_error();
                                        fprintf( stderr , "function %s is not defined\n" , id );
                                        exit(0);
                                }
                                int fun_loc = func_list_loc[get_func_index(id)];
                                while(parse(src) != ')');
                                if(debug)fprintf( stderr , "CALL\t%d\n" , fun_loc - 2);
                                _asm(OP_CALL , new_var( INT , (var_union)(fun_loc - 2)));
                                return 1;
                            }
                            else { 
                                pos = start_pos; 
                                if (in_func) 
                                {
                                    if(debug)fprintf( stderr , "LLV\t%s\n" , id );
                                    _asm(OP_LLV , new_var( STRING , (var_union)id));
                                    if(get_index_in_table(Local , id) == -1 && get_index_in_table(Global , id) == -1)
                                    {
                                        parse_error();
                                        fprintf( stderr , "name %s is not declared in this function or in the main chunk\n" , id );
                                        exit(0);
                                    }
                                }
                                else 
                                {
                                    if(get_index_in_table(Global , id) == -1)
                                    {
                                        parse_error();
                                        fprintf( stderr , "name %s is not declared\n" , id );
                                        exit(0);
                                    }
                                    if(debug)fprintf( stderr , "LGV\t%s\n" , id );
                                    _asm(OP_LGV , new_var( STRING , (var_union)id));
                                }
                                id = pre_id; 
                                return 1;
                            }

		case STRING_VAL:	if(debug){ fprintf( stderr , "LSTR\t\""); print_lstr(lextext); fprintf( stderr , "\"\n"); } 
                            _asm(OP_LSTR , new_var( STRING , (var_union)strdup(lextext)));
                            return 1;

        case         IF:    
                            //level = max_lvl+1; 
                            in_cond = 1;
                            int a = parse();
                            while(a != THEN)
                            {
                                int cur_line = cur_line_start_pos;
                                if(!a)
                                {
                                    cur_line_start_pos = cur_line;
                                    parse_error();
                                    fprintf( stderr , "expected 'then' before end of line\n" );
                                    exit(0);
                                }
                                a = parse();
                            } 
                            in_cond = 0;
                            int dummy = chunk_buf_pos;
                            if(debug)fprintf( stderr , "JZ\t%d\n" , 0 );  
                            _asm(OP_JZ , new_var( INT , (var_union)(0) ));
                            //int cur_pos = pos;
                            //int tmp_counting = counting , tmp_debug = debug; debug = 0;
                            //counting = 1;
                            //tmp_inst_count = inst_count;
                            while(parse(src) != END); 
                            //counting = tmp_counting;
                            //debug = tmp_debug;
                            int if_block_size = (chunk_buf_pos - dummy)/10 - 1;
                            memcpy( &chunk_buf[dummy+2] , &if_block_size , sizeof(int));
                            
                            //pos = cur_pos;
                            //while(parse(src) != END);
                            //inst_count -= if_block_size;
                            return 0;

        case      WHILE:    
                            //level = max_lvl+1; 
                            //if(debug)fprintf( stderr , "w%d:\n", wh_lvl++ ); 
                            in_cond = 1;
                            dummy = chunk_buf_pos;
                            while(parse(src) != DO); 
                            int tdum = chunk_buf_pos;
                            if(debug)fprintf( stderr , "JZ\t%d\n" , 0 );  
                            _asm(OP_JZ , new_var( INT , (var_union)(0) ));
                            in_cond = 0;
                            //tmp_inst_count = inst_count;
                            //counting = 1; int tmp_debug = debug; debug = 0;
                            //while(parse(src) != END); 
                            //counting = 0; debug = tmp_debug;
                            //inst_count = tmp_inst_count + 1
                            //pos = cur_pos;
                            while(parse(src) != END); 
                            int while_block_size = (chunk_buf_pos - dummy)/10 + 1;
                            if(debug)fprintf( stderr , "JMP\t%d\n" , -while_block_size);
                            _asm(OP_JMP , new_var(INT , (var_union)(-while_block_size))); 
                            int while_size = (chunk_buf_pos - tdum)/10 - 1;
                            memcpy( &chunk_buf[tdum+2] , &while_size , sizeof(int) ); 
                            //if(debug)fprintf( stderr , "%d:\n" , --level); 
                            return 0;

        case       THEN:    return THEN;
        case         DO:    return DO;
        case        END:    return END;
        case       FUNC:    in_func = 1;
                            dummy = chunk_buf_pos;
                            /*cur_pos = pos;
                            counting = 1; tmp_debug = debug; debug = 0;
                            parse_func(src); 
                            while(parse(src) != END); 
                            counting = 0;
                            int jmp_v = inst_count - dummy;
                            inst_count = dummy;
                            debug = tmp_debug;
                            */
                            if(debug)fprintf( stderr , "JMP\t%d\n" , 0);
                            _asm(OP_JMP , new_var(INT , (var_union)(0)));
                            if(debug)fprintf( stderr , "LLT\t%d\n" , 0);
                            _asm(OP_LLT , new_var(INT , (var_union)(0)));
                            //pos = cur_pos;
                            parse_func(src); 
                            while(parse(src) != END); 
                            int func_size = (chunk_buf_pos - dummy)/10;
                            memcpy( &chunk_buf[dummy+2] , &func_size , sizeof(int) );
                            memcpy( &chunk_buf[dummy+12] , &(Local->count) , sizeof(int) );
                            if(debug)fprintf( stderr , "RET\n"); 
                            _asm(OP_RET , NULL);
                            in_func = 0;
                            return 0;

        case     RETURN:    while(parse());
                            if(debug)fprintf( stderr , "RET\n");
                            _asm(OP_RET , NULL);
                            return 1;
    }
    return 0;
}

int parse()
{
    token.pre.token = 0;
    token.cur.token = 0;
    token.nxt = get_token();
    parse_prgrm();
}

/*
int main(int argc , char * argv[])
{
    if(argc < 2 || argc > 4) 
    {
        fprintf( stderr , "usage : %s [-d debug_level] filename\n" , argv[0] );
        exit(0);
    }
    int c = 0;
    while(++c < argc)
    {
        if(!strcmp(argv[c] , "-d")) //Checking debug level
        {
            debug = atoi(argv[++c]);
            continue;
        }
        src = open_file(argv[c]);
    }
    init_int();
    if(debug)fprintf( stderr , ".------------------------.\n|      CODE SECTION      |\n'------------------------'\n");
    while(pos < strlen(src))
    {
        parse(src);
    }
    _asm(OP_HALT , new_var(NULL_T , (var_union)(0)));
    if(debug > 4)fprintf( stderr , ".------------------------.\n|      DATA SECTION      |\n'------------------------'\n");
    int i;
    for(i=0;i<Strings->count;i++)
    {
        if(debug > 4)fprintf( stderr , "%d:\t\"%s\"\n" , i , Strings->var[i].var.val.char_t );
    }
    //if(debug < 50)exec(chunk_buf , Strings);
    str_table_t * str_table = encode_str_table(Strings);
    char header[] = { 0 , 0 , 0 , 0 };
    FILE * out = fopen(".out" , "w");
    fwrite( &header , 4 , 1 , out);
    int inst_size = inst_count * 10;
    fwrite( &inst_size , sizeof(int) , 1 , out);
    fwrite( &Global->count , sizeof(int) , 1 , out);
    fwrite( chunk_buf , inst_count*10 , 1 , out);
    fwrite( str_table->slots , (Strings->count + 1) * sizeof(int) , 1 , out);
    fwrite( str_table->str , strlen(str_table->str) , 1 , out);
    int file_size = ftell(out);
    fwrite( &file_size , sizeof(int) , 1 , out);
    fclose(out);
    if(debug)fprintf( stderr , "\n");
    return 0;
}
*/

typedef struct 
{
    char * src ;
    int globals_count ; 
    element_table_t * strings ;
    table_t * globals ; /* Optional debug data*/
    int inst_count;
}parse_return;

parse_return parse_file(char * filename)
{
    
    input_file_name = filename;
    src = open_file(filename);
    init_int();
    //while(pos < strlen(src))
        parse();
    _asm(OP_HALT , new_var(NULL_T , (var_union)(0)));
    parse_return ret = 
    {
        .src = chunk_buf ,
        .globals_count = Global->count ,
        .strings = Strings ,
        .globals = Global ,
        .inst_count = inst_count
    };
    return ret;

}