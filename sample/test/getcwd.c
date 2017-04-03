#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <string.h>

int main(){
    char buf[100];
    getcwd(buf, 100);
    printf("current directory is: %s\n", buf);
}