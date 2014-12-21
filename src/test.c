#include "include/var.h"
#include "include/cmp.h"

int main(int argc, char const *argv[])
{
	element_var_t var1 = {.type = INT  , .val = 1};
	element_var_t var2 = {.type = INT  , .val = 1};
	printf("%d\n" , alu_tlt(&var1 , &var2) );
	return 0;
}