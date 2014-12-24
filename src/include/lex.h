#ifndef _LEX_H
#define _LEX_H


#define LEXEOF 256
#define INT_VAL 257
#define ID 258
#define IF 259
#define THEN 260
#define END 261
#define STRING_VAL 262
#define DOUBLE_VAL 263
#define EQ	264
#define LE	267
#define GE	268
#define NE	269
#define WHILE	270
#define DO	271
#define FUNC	272
#define FUNC_CALL	273
#define RETURN 	274
#define FOR 	275
#define ASSIGN 	275
#define CMNTEND	276

char * token2string[] =
{
	"end of file" , 0 , 0 , "if" , "then" , "end" , 0 , 0 , "=" , "<=" , ">=" , "!=" , "while" ,
	"do" , 0 , 0 , "return" , "for" , "=" , 0 , 0 , "=" , "<=" , ">=" , "!="
};

#define MAX_FUNC_COUNT 512
char * func_list_name[MAX_FUNC_COUNT];
int    func_list_loc[MAX_FUNC_COUNT];
int cur_func_index = 5;

int get_func_index(char * name){int i=-1;while( ++i < cur_func_index)if(!strcmp(func_list_name[i],name))return i;return -1;}
char * get_func_name(int loc){int i=-1;while( ++i < cur_func_index)if(func_list_loc[i] == loc)return func_list_name[i];return '\0';}

int is_digit(char c) {return (c >='0' && c <='9')?1:0;}
int is_alpha(char c) {return ( (c >='a' && c <='z') || (c >='A' && c <='Z') )?1:0;}
int is_valid_id_start(char c) {return (is_alpha(c)||c=='_')?1:0;}
int is_valid_id_name(char c) {return (is_alpha(c)||c=='_'||is_digit(c))?1:0;}
char * lextext;
char * pre_id , *id;
int level = 0 , max_lvl = 0 , wh_lvl = 0;
int in_func , in_cond , tmp_inst_count , in_table = 0;
int debug = 0;

void empty(char * buf , int size)
{
	while(buf[--size] = 0);
}

char * open_file(char * filename)
{
    FILE * file = fopen( filename , "r");
    if(!file)
	{
		fprintf( stderr , "error : can't open file '%s' , file was not found\n" , filename );
		chalk_exit();
	}
    fseek(file , 0L , SEEK_END );
    int size = ftell(file);
    fseek(file , 0L , SEEK_SET);
    char * buf = _calloc(size , sizeof(char));
    fread(buf , size , 1 , file);
    fclose(file);
    return buf;
}

char * fstrndup(char * src , int size)
{
	char * ret = _calloc(size+1,sizeof(char));
	int i , j=0;
	for(i=0;i<size;i++)
	{
		if(src[i] == '\\')
			switch(src[++i])
			{
				case 'n':	ret[j++] = '\n'; continue;
			}
		ret[j++] = src[i];
	}
	return ret;
}

void print_lstr(char * str)
{
	int i = 0;
	while(str[i] != '\0')
	{
		switch(str[i])
		{
			case '\n': fprintf( stderr , "\\n"); break;
			 default : fprintf( stderr , "%c" , str[i] ); break;
		}
		i++;
	}
}

typedef struct 
{
	int * slots;
	char * str;
}str_table_t;

str_table_t * encode_str_table(element_table_t * Strings)
{
	str_table_t * str_table = _malloc(sizeof(str_table_t));
	int * slots = _calloc(Strings->count,sizeof(int));
	slots[0] = Strings->count;
	int cur_offset = 0;
	int i;
	for(i=0;i<Strings->count;i++)
	{
		cur_offset += strlen(Strings->var[i].val.char_t);
		slots[i+1] = cur_offset;
	}
	char * str = _calloc(cur_offset+1 , sizeof(char));
	int si , s , sn = 0;
	for(si=0;si<i;si++)
	{
		for(s=0;s<strlen(Strings->var[si].val.char_t);s++)
		{
			str[sn++] = Strings->var[si].val.char_t[s];
		}
	}
	str[cur_offset] = '\0';
	str_table->slots = slots;
	str_table->str = str;
	return str_table;
}

typedef struct 
{
	int line_occured_start;
	int pos;
	int posx;
	int posy;
	int token;
	char * text;
}tok;

typedef struct 
{
	tok pre;
	tok cur;
	tok nxt;
}token_t;

token_t token;

char * tokentostring(tok token);

#endif