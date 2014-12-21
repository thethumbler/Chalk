#ifndef _LEXER_C
#define _LEXER_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "include/table.h"
#include "include/lex.h"
#include "stack.c"
#include "vm.c"
#include "asm.c"
#include "include/api.h"

int is_in(char c , char *str)
{
	int i;
	for(i=0;i<strlen(str);i++)
		if(c == str[i])
			return 1;
	return 0;
}

char * src;
int start_pos, pos = 0 , pos_y = 1 ,cur_line_start_pos = 0;

int get_token_value()
{
    char * str = src; 
    if (str[pos] == '\0')  return LEXEOF; /* Returns end of file */
   	if(str[pos] == ' ' ||str[pos] == '\t')
   	{
   		while(str[pos] == ' ' ||str[pos] == '\t') pos++;   /* Ignoring whitespace */
   		return get_token_value();
   	}
    
    if(!strncmp( &str[pos] , "//" , 2)) 
    {
        pos += 2;
        while( str[pos] != '\n' && str[pos++] != '\0');    // Ignoring line comments
    }

    if(!strncmp( &str[pos] , "/*" , 2)) 
    {
        pos += 2;
        while(pos++)    /* Ignoring block comments */
        {
        	if(str[pos] == '*')
        		if(str[++pos] == '/')
        			return get_token_value(pos++);
        	if(str[pos] == '\0') return LEXEOF;
        }
    }

    start_pos = pos;    /* Keeping reference of the previous token */
    if ( str[pos] == ';' ) return str[pos++]; /* Optional semi-colon (;) support */
    else if ( str[pos] == '=' ) /* Checks the equal sign (=) */
    {
        pos++;
        if(in_cond) return EQ;  /* Returns equality compariosn if in condition */
        return ASSIGN;          /* Returns assingment operation */
    }
    
    else if ( !strncmp(&str[pos] , "*/" , 2))   /* Used for block comments */
    {
        pos += 2;
        return CMNTEND;
    }
    
    else if ( is_in(str[pos] , "!()+-*/<>,^[]{}\n") )    /* These characters are checked with their ASCII value */
    {
        switch(str[pos])
        {
            case '>':   if(str[pos+1] == '=') { pos++; return GE; } break; /* Returns greater than or equal comparison */
            case '<':   if(str[pos+1] == '=') { pos++; return LE; } break; /* Returns less than or equal comparison */
            case '!':   if(str[pos+1] == '=') { pos++; return NE; } break; /* Returns not equal comparison */
        }
        return str[pos++];  /* Otherwise return what was found */
    }

    else if (is_digit(str[pos]))    /* Checks if a number is found */
    {
        while(is_digit(str[pos]))pos++; /* Skips until the first not number character appears */
        if(str[pos] == '.')     /* If it has a dot in it .. then it's a floating point number .. saved as double */
        {
            pos++;
            while(is_digit(str[pos]))pos++;
            lextext = strndup( &str[start_pos] , pos-start_pos );
            return DOUBLE_VAL;
        }
        lextext = strndup( &str[start_pos] , pos-start_pos );
        return INT_VAL;
    }

    else if (is_valid_id_start(str[pos]))    /* is either a keyword or an identifier */
    {
        while(is_valid_id_name(str[pos]))pos++; /* Skipps characters , digits and underscores */
        lextext = strndup( &str[start_pos] , pos-start_pos );
        int len = strlen(lextext);
        /* Checking if it's a keyword */
        if  	(!strcmp(lextext, "if"         ) && len == 2)
        	return IF;
        else if (!strcmp(lextext, "then"       ) && len == 4)
        	return THEN;
        else if (!strcmp(lextext, "end"        ) && len == 3)
        	return END;
        else if (!strcmp(lextext, "while"      ) && len == 5)
            return WHILE;
        else if (!strcmp(lextext, "do"         ) && len == 2)
            return DO;
        else if (!strcmp(lextext, "function"   ) && len == 8)
            return FUNC;
        else if (!strcmp(lextext, "return"     ) && len == 6)
            return RETURN;
        else if (!strcmp(lextext, "for"        ) && len == 3)
            return FOR;
        return ID;  /* Otherwise it's an identifier */
    }

    else if ( str[pos] == '"' )
    {
    	pos++;
    	while(str[pos] != '"') pos++;
    	lextext = fstrndup( &str[start_pos+1] , pos - start_pos -1);
    	pos++;
        return STRING_VAL;
    }

    /*
    else if ( str[pos] == '"' )
    {
        pos++;
        while(str[pos] != '"')pos++;
        lextext = strndup( &str[start_pos] , pos - start_pos);
        pos++;
        return STRING_VAL;
    }
    */

    pos++;
	/*
    /* Unexpected symbol in the source code 
    fprintf( stderr , "unexpected symbol in line %d:\n" , pos_y );
    /* Printing from cur_line_start_pos until new line 
    int s_c = pos - cur_line_start_pos - 1;
    while(str[cur_line_start_pos] != '\n')
        fprintf( stderr , "%c" , str[cur_line_start_pos++] );
    fprintf(stderr, "\n");
    int m;
    for(m=0; m < s_c ; m++)
        fprintf(stderr , " ");
    fprintf(stderr , "^\n");
    exit(0);
    */
}

tok get_token()
{
    tok ret;
    ret.pos = pos;
    ret.posx = pos - cur_line_start_pos;
    ret.posy = pos_y;
    ret.line_occured_start = cur_line_start_pos;    
    ret.token = get_token_value();
    ret.text = lextext;
    return ret;
}

int lex()
{
    if(token.cur.token == '\n') 
    {
        pos_y++;
        cur_line_start_pos = start_pos;
    }
    token.pre = token.cur;
    token.cur = token.nxt;
    token.nxt = get_token();
}

#endif