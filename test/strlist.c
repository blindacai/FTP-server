#include <stdio.h>
#include <string.h>

void changeString(char* str4){
    str4[0] = 'y';
}

int main(){
    printf("in string list main\n");

    char* str_list[2];

    str_list[0] = "one";
    str_list[1] = "two";

    printf("list: %s\n", str_list[0]);
    printf("list: %s\n", str_list[3]);   // no boundary check

    // pointer to a list of 4 pointers
    char* str_list2[4] = {"aaa", "def", "ghi"};
    printf("list2: %s\n", str_list2[0]);
    printf("list2: %s\n", str_list2[2]);
    printf("size of list2: %d\n", sizeof(str_list2) / sizeof(str_list[0]));  // 32/8
    printf("addr of str_list2: %p\n", (void*) &str_list2);
    printf("addr of str_list2[0]: %p\n", (void*) &str_list2[0]);
    printf("addr of str_list2[1]: %p\n", (void*) &str_list2[1]);
    printf("char of str_list2: %c, %c\n", str_list2[0][0], str_list2[0][1]);
    printf("addr of chars: %p, %p\n", (void*) &str_list2[0][0], (void*) &str_list2[0][1]);

    char* a_str = "ddd";
    printf("a_str: %s\n", a_str);

    char chars[3] = {'x', 'y', 'z'};
    printf("chars: %c\n", chars[0]);
    printf("addr of chars: %p\n", (void*) &chars);
    printf("addr of chars[1]: %p\n", (void*) &chars[1]);

    // ! ignore this part
    char** str_list3;
    str_list3[0] = "asd";
    str_list3[1] = "lkj";
    str_list3[2] = "poi";
    str_list3[3] = NULL;
    printf("str_list3 0: %s\n", str_list3[0]);
    printf("str_list3 3: %s\n", str_list3[3]);
    // ! may cause seg fault   // should explicitly define the length: char* str_list3[4]

    char* str1 = "abcd\n";
    printf("len of string: %d\n", strlen(str1));

    char str3[10] = "abcd\n";
    printf("str length: %d\n", strlen(str3));  //5 actual size
    printf("str size: %d\n", sizeof(str3));  //10

    char str2[6] = "apple\n";
    memset(str2, 0, 10);
    printf("reset string: %s\n", str2);

    char str4[5] = "lkjh\n";
    changeString(str4);     // pass by reference
    printf("new str4: %s\n", str4);

    char str5[10] = "abc a\0bc";
    printf("len of str6: %d\n", strlen(str5));
}