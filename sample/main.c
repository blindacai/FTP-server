#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "server.h"

int main(int argc, char **argv){
    listenOnConnect(argv[1]);
    acceptConnect();
}