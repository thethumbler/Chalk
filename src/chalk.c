#include "parser.c"
#include "runtime.c"
#include "repl.c"

#define CHALK_VERSION "Chalk VM version : 0.0.1 (Alpha)"

void print_message () {
  	fprintf(stderr,
  	"usage: chalk [options].  Available options are:\n"
  	"  -c [file]  compile only\n"
  	"  -o [file]  specify output file (default chalk.out)\n"
  	"  -v       	print version information\n" 
	);
  	exit(0);
}

void print_version (void) {
  	printf("%s\n" , CHALK_VERSION);	/* Notice that the version is a string and not just a number */
	exit(0);
}

char * out_error()
{
	print_message();
	return NULL;
}

int main(int argc , char * argv[])
{
	repl();
	exit(0);

	//printf("int = %ld , long = %ld , double = %ld , long double = %ld\n" , sizeof(int) , sizeof(long) , sizeof(double) , sizeof(long double) );

	if(argc < 2) print_message();
	int i , compile = 0;
	char * input = NULL;
	char * output = "chalk.out";
	for(i = 1 ; i < argc ; i++ )
	{
		if(argv[i][0] == '-')
		{
			switch(argv[i][1])
			{
				case 'c': compile = 1; continue; 
				case 'o': output = (argc-i)?argv[++i]:out_error(); continue;
				case 'v': print_version();
				default : print_message();
			}
		}
		if(!input)
		{ 
			input = argv[i];
			continue;
		}
		print_message();
	}
	
	/* Checking if binary file */ 
    {
    	FILE * in = fopen(input , "r");
    	char src[4];
    	fread(src , 4 , 1 , in);
    	fclose(in);
    	if(check_header(src))
    	{
    		launch_exec(open_file(input));
    		exit(0);
    	}
    }

    parse_return executable = parse_file(argv[1]);
    if(compile)
    {
    	str_table_t * str_table = encode_str_table(Strings);
    	char header[] = { 'C' , 'H' , 'K' , 0x21 };
    	FILE * out = fopen( output , "w");
    	fwrite( &header , 4 , 1 , out);
    	int inst_size = executable.inst_count * 10;
    	fwrite( &inst_size , sizeof(int) , 1 , out);
    	fwrite( &executable.globals->count , sizeof(int) , 1 , out);
    	fwrite( chunk_buf , inst_count*10 , 1 , out);
    	fwrite( str_table->slots , (Strings->count + 1) * sizeof(int) , 1 , out);
    	fwrite( str_table->str , strlen(str_table->str) , 1 , out);
    	int file_size = ftell(out);
    	fwrite( &file_size , sizeof(int) , 1 , out);
    	fclose(out);
    	exit(0);
    }

    launch(executable.src , executable.globals_count , executable.strings , executable.globals);
    return 0;
}