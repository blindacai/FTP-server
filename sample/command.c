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


void printfCommands(char** commands){
	char** offset = commands;
	while(*offset){
		printf("offset is %s\n", *offset);
		offset++;
	}
}


void printfString(char* string){
	char* offset = string;
	while(*offset != '\0'){
		printf("print char: %c\n", *offset);
		offset++;
	}
}


void sendMsg(int new_fd, char* message){
	send(new_fd, message, strlen(message), 0);
}


// argv is a string
// return true if the string ends with a new line
bool checkForNewLine(char* str){
	int i;
	int length = strlen(str);
	char* offset = str;

	for(i = 0; i < length; i++){
		if(*(offset + i) == '\n'){
			return true;
		}
	}
	return false;
}


// argv: a list of string
// return array length
int arr_len(char** str_array){
	char** offset;
    for(offset = str_array; *offset != NULL || *offset == 0; ++offset){
		if(checkForNewLine(*offset)){
			break;
		}
	}
    return offset - str_array + 1;
}


void remove_endofstring(char* string){
	int length = strlen(string);
	string[length - 1] = 0;
}


void remove_endofline(char** commands){
	if(!checkForNewLine(commands[0])){
		return;
	}

	if(arr_len(commands) == 1){
		remove_endofstring(commands[0]);
		return;
	}
	else{
		remove_endofstring(commands[1]);
		return;
	}
}

void response(int new_fd, char** commands){
	//printf("commands: %s\n", commands[0]);
	remove_endofline(commands);

	//sendMsg(new_fd, "230 Login successful.\n\rRemote system type is UNIX.\n\rUsing binary mode to transfer files.\n\r");
	//sendMsg(new_fd, "230 Login successful.\n\r");

	if(strcmp(commands[0], "USER") == 0){
		sendMsg(new_fd, "230 Login successful.\n\r");
	}

	else{
		sendMsg(new_fd, "invalid commands.\n\r");
	}
}