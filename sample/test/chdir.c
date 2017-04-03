#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <string.h>

#include "../dir.h"

int main(){
    char* directory = "callfun";
    int result = listFiles(1, directory);
    printf("list dir: %d\n", result);
}