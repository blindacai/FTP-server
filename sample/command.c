#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdbool.h>

void sendMsg(int new_fd, char* message){
	send(new_fd, message, strlen(message), 0);
}

// argv is a string
// return true if the string ends with a new line
bool checkForNewLine(char* str){
	int i;
	int length = strlen(str);

	for(i = 0; i < length; i++){
		if(*(str + i) == '\n'){
			return true;
		}
	}
	return false;
}


// argv: a list of string
// return array length
int arr_len(char** str_array){
	char** offset;
    for(offset = str_array; *offset != NULL; ++offset){
		if(checkForNewLine(*offset)){
			break;
		}
	}
    return offset - str_array + 1;
}


char** remove_endofline(char** commands){

}

void response(int new_fd, char** commands){
    sendMsg(new_fd, "230 Login successful.\n");
}