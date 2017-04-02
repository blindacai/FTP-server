#include <stdio.h>
#include <string.h>
#include "funB.h"

int main(){
    char* name[5] = {};
     name[0] = "apple";
     name[1] = "juice";
     name[2] = "pie";
     printf("print name one: %s\n", name[0]);
     printf("print name two: %s\n", name[1]);
     printf("length of string: %d\n", str_len(name));
}