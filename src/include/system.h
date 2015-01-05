
#ifndef _SYSTEM_H
#define _SYSTEM_H

typedef char flag;

flag debug = 0;
flag PARSE_ERROR_FLAG = 0;
flag IN_REPL = 0; 

#include <stdint.h>
#include <stdio.h>
#include "mem.h"

void chalk_exit()
{
	//_free_alloc();
    if(IN_REPL) repl_loop();
	exit(0);
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
    char * buf = calloc(size , sizeof(char));
    fread(buf , size , 1 , file);
    fclose(file);
    return buf;
}


#endif