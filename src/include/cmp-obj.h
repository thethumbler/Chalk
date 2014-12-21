#ifndef CMP_H
#define CMP_H
int alu_tlt(element_var_t * var1 , element_var_t * var2)
{
	if((*var1).type == INT && (*var2).type == INT) 
		if( (*var2).val.int_t < (*var1).val.int_t ) 
			return 0; 
	else if((*var1).type == INT && (*var2).type == DOUBLE) 
		if( (*var2).val.double_t < (*var1).val.int_t ) 
			return 0;
	else if((*var1).type == DOUBLE && (*var2).type == INT) 
		if( (*var2).val.int_t < (*var1).val.double_t ) 
			return 0;
	else if((*var1).type == DOUBLE && (*var2).type == DOUBLE) 
		if( (*var2).val.double_t < (*var1).val.double_t ) 
			return 0;
	return 1;
}

int alu_tgt(element_var_t * var1 , element_var_t * var2)
{
	if((*var1).type == INT && (*var2).type == INT) 
		if( (*var2).val.int_t > (*var1).val.int_t ) 
			return 0; 
	else if((*var1).type == INT && (*var2).type == DOUBLE) 
		if( (*var2).val.double_t > (*var1).val.int_t ) 
			return 0;
	else if((*var1).type == DOUBLE && (*var2).type == INT) 
		if( (*var2).val.int_t > (*var1).val.double_t ) 
			return 0;
	else if((*var1).type == DOUBLE && (*var2).type == DOUBLE) 
		if( (*var2).val.double_t > (*var1).val.double_t ) 
			return 0;
	return 1;
}

int alu_tle(element_var_t * var1 , element_var_t * var2)
{
	if((*var1).type == INT && (*var2).type == INT) 
		if( (*var2).val.int_t <= (*var1).val.int_t ) 
			return 0; 
	else if((*var1).type == INT && (*var2).type == DOUBLE) 
		if( (*var2).val.double_t <= (*var1).val.int_t ) 
			return 0;
	else if((*var1).type == DOUBLE && (*var2).type == INT) 
		if( (*var2).val.int_t <= (*var1).val.double_t ) 
			return 0;
	else if((*var1).type == DOUBLE && (*var2).type == DOUBLE) 
		if( (*var2).val.double_t <= (*var1).val.double_t ) 
			return 0;
	return 1;
}

int alu_tge(element_var_t * var1 , element_var_t * var2)
{
	if((*var1).type == INT && (*var2).type == INT) 
		if( (*var2).val.int_t >= (*var1).val.int_t ) 
			return 0; 
	else if((*var1).type == INT && (*var2).type == DOUBLE) 
		if( (*var2).val.double_t >= (*var1).val.int_t ) 
			return 0;
	else if((*var1).type == DOUBLE && (*var2).type == INT) 
		if( (*var2).val.int_t >= (*var1).val.double_t ) 
			return 0;
	else if((*var1).type == DOUBLE && (*var2).type == DOUBLE) 
		if( (*var2).val.double_t >= (*var1).val.double_t ) 
			return 0;
	return 1;
}

int alu_teq(element_var_t * var1 , element_var_t * var2)
{
	if((*var1).type == INT && (*var2).type == INT) 
		if( (*var2).val.int_t == (*var1).val.int_t ) 
			return 1; 
	else if((*var1).type == INT && (*var2).type == DOUBLE) 
		if( (*var2).val.double_t == (*var1).val.int_t ) 
			return 1;
	else if((*var1).type == DOUBLE && (*var2).type == INT) 
		if( (*var2).val.int_t == (*var1).val.double_t ) 
			return 1;
	else if((*var1).type == DOUBLE && (*var2).type == DOUBLE) 
		if( (*var2).val.double_t == (*var1).val.double_t ) 
			return 1;
	return 0;
}

int alu_tne(element_var_t * var1 , element_var_t * var2)
{
	if((*var1).type == INT && (*var2).type == INT) 
		if( (*var2).val.int_t != (*var1).val.int_t ) 
			return 0; 
	else if((*var1).type == INT && (*var2).type == DOUBLE) 
		if( (*var2).val.double_t != (*var1).val.int_t ) 
			return 0;
	else if((*var1).type == DOUBLE && (*var2).type == INT) 
		if( (*var2).val.int_t != (*var1).val.double_t ) 
			return 0;
	else if((*var1).type == DOUBLE && (*var2).type == DOUBLE) 
		if( (*var2).val.double_t != (*var1).val.double_t ) 
			return 0;
	return 1;
}
#endif