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

int new_fd;
bool loggedin = false;


void set_newfd(int newfd){
	new_fd = newfd;
}

void resetLogin(){
	loggedin = false;
}


void sendMsg(char* message){
	send(new_fd, message, strlen(message), 0);
}


void invalid(){
	sendMsg("Invalid Command.\n\r");
}


// for testing
void printfCommands(char** commands){
	char** offset = commands;
	while(*offset){
		printf("offset is %s\n", *offset);
		offset++;
	}
}


// for testing
void printfString(char* string){
	char* offset = string;
	while(*offset != '\0'){
		printf("print char: %c\n", *offset);
		offset++;
	}
}


// argv is a string
// return true if the string ends with a new line
bool checkForNewLine(char* str){
	int i;
	int length = strlen(str);
	char* offset = str;

	for(i = 0; i < length; i++){
		char current = *(offset + i);
		if(current == '\n' || current == '\r'){
			return true;
		}
	}
	return false;
}


// argv: an array of string
// return array length
int arr_len(char** str_array){
	char** offset;
    for(offset = str_array; *offset != NULL; ++offset);
    return offset - str_array;
}


// remove \r or \n from the end of a string
void remove_endofstring(char* string){
	int length = strlen(string);
	char last = string[length - 1];
	char second_last = string[length - 2];
	
	if(last == '\n' || last == '\r'){
		string[length - 1] = 0;
	}
	if(second_last == '\n' || second_last == '\r'){
		string[length - 2] = 0;
	}
}


// remove \r or \n from the end of user commands
void remove_endofline(char** commands){
	if(arr_len(commands) == 1){
		remove_endofstring(commands[0]);
		return;
	}
	else{
		remove_endofstring(commands[1]);
		return;
	}
}


// process 'user cs317' command
void user(char* username){
	if(loggedin){
		invalid();
		return;
	}

	if(strcmp(username, "cs317") == 0){
		sendMsg("230 Login successful.\n\r");
		loggedin = true;
	}
	else{
		sendMsg("530 This FTP server is cs317 only.\n\r");
	}
}

void type(char* thetype){
	if(strcmp(thetype, "I") == 0){
		sendMsg("200 Switching to Binary mode.\n\r");
	}
	else if(strcmp(thetype, "A") == 0){
		sendMsg("200 Switching to ASCII mode.\n\r");
	}
	else{
		invalid();
	}
}


// create server response
void response(char** commands){
	if(strcmp(commands[0], "USER") == 0){
		user(commands[1]);
	}
	else if(strcmp(commands[0], "TYPE") == 0){
		type(commands[1]);
	}
	else if(strcmp(commands[0], "MODE") == 0){
		sendMsg("We only support stream mode, sorry.\n\r");
	}
	else if(strcmp(commands[0], "STRU") == 0){
		sendMsg("We only support file structure, sorry.\n\r");
	}
	else{
		invalid();
	}
}