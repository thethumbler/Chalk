char * str_dup(char * src , int size)
{
    char * str = calloc(size , sizeof(char));
    int i;
    for(i=0;i<size;i++)
        str[i] = src[i];
    return str;
}

int str_len(char * src)
{
    int i=0;
    while(src[i] != '\0')i++;
    return i;
}