#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <string.h>

#include "../dir.h"

int main(){
    char* directory = "callfun";
    listFiles(1, directory);
    //int result = listFiles(1, directory);
    //printf("list dir: %d\n", result);

    printf("now cd into callfun\n");
    chdir(directory);
    char* inner_dir = "innerdir";
    listFiles(1, "./");

    printf("now cd into innerdir\n");
    chdir("innerdir");
    listFiles(1, "./");

    char* dir_notexist = "outerdir";
    int error = chdir(dir_notexist);
    printf("cd into non-exist dir: %d\n", error);

    char* to_parent = "../";
    int parent = chdir(to_parent);
    printf("cd into parent dir: %d\n", parent);
    listFiles(1, "./");
}