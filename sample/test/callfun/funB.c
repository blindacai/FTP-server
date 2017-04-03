#include <stdio.h>
#include <string.h>

int str_len(char** string){
    char** offset;
    for(offset = string; *offset != NULL; ++offset);
    return offset - string;
}

int test_refer(){
    return 10;
}