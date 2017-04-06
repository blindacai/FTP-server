#include <stdio.h>
#include <string.h>

int main(){
    printf("in string compare main\n");

    char* str1 = "hello";
    char* str2 = "hello";
    char str3[] = "hello";

    char buf[512];
    void* middle = buf;
    middle = "hello";

    // first arg of strtok() should be modifiable; "string literal" is read only
    char str4[20] = "hello;one";
    str4[9] = 'w';
    printf("string 4: %s\n", str4);
    printf("parse: %s\n", strtok(str4, ";"));
    char* piece = strtok(str4, ";");

    printf("compare: %d\n", strcmp(str1, str2));
    printf("compare: %d\n", strcmp(str1, str3));
    printf("compare: %d\n", strcmp(str1, middle));
    printf("compare: %d\n", strcmp(str1, piece));

    char* user = "user";
    char* quit = "quit";
    printf("compare: %d\n", strcmp(user, quit));

    char str5[512] = "quit\n";
    char* str6 = "quit";
    char* str7 = strtok(str5, " ");
    printf("compare: %d\n", strcmp(str7, str6));

}