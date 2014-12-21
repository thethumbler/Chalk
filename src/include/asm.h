#ifndef ASM_H
#define ASM_H
void print_hex(char * data , int len)
{
	int i;
	for(i=0;i<len;i++)
	{
		printf("%x%x ", 0xF & (data[i]>>4) , 0xF & data[i]);
	}
}

char chunk_buf[65536];
int chunk_buf_pos = 0;
void write_chunk(char * chunk)
{
	memcpy(&chunk_buf[chunk_buf_pos],chunk,10);
	chunk_buf_pos +=10;
}
#endif