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

#include <regex.h>
#include <sys/types.h>

#include "command.h"
#include "dir.h"

int new_fd;
bool loggedin = false;
char root_dir[100];


void set_rootdir(char* root){
	strcpy(root_dir, root);
	printf("root dir: %s\n\r", root_dir);
}

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
	sendMsg("500 Invalid Command.\n\r");
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
		sendMsg("You are already logged in\n\r");
		return;
	}
	if(username == NULL){
		sendMsg("530 Please provide username\n\r");
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


// process 'type image' command
void type(char* thetype){
	if(thetype == NULL){
		sendMsg("Transfer type not specified\n\r");
		return;
	}

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


// return true if not allowed pattern detected
bool notallow_match(char* dir){
	regex_t regex;
    int reti;
	reti = regcomp(&regex, "/{0,1}.{0,1}[.]/{0,1}", REG_EXTENDED);

	if(reti){ 
		fprintf(stderr, "Could not compile regex\n\r"); 
    }

	// execute
	reti = regexec(&regex, dir, 0, NULL, 0);
	return !reti;
}


// process 'cwd local' command
void cdinto(char* dir){
	if(dir == NULL){
		sendMsg("Please provide the directory\n\r");
		return;
	}

	if(notallow_match(dir)){
		sendMsg("550 Not allowed to change directory this way.\n\r");
		listFiles(1, "./");            // for testing
	}
	else{
		if(chdir(dir) == 0){
			sendMsg("250 Directory successfully changed.\n\r");
		}
		else{
			sendMsg("550 Failed to change directory.\n\r");
		}
	
		listFiles(1, "./");            // for testing
	}
}


// process 'MODE' command
void switchmode(char* mode){
	if(mode == NULL){
		sendMsg("Transfer mode not specified\n\r");
	}
	else{
		sendMsg("We only support stream mode, sorry.\n\r");
	}
}


// process 'STRU' command
void switchstru(char* stru){
	if(stru == NULL){
		sendMsg("File structure not specified\n\r");
	}
	else{
		sendMsg("We only support file structure, sorry.\n\r");
	}
}


// return true if current dir is the root dir
bool in_rootdir(){
	char current[100];
	getcwd(current, 100);
	return !strcmp(current, root_dir);
}


// process 'CDUP' command
void cdto_parent(){
	if(in_rootdir()){
		sendMsg("Cannot change directory. Already in root\n\r");
	}
	else{
		chdir("../");
		sendMsg("250 Directory successfully changed.\n\r");
		listFiles(1, "./");              // for testing
	}
}


// create server response
void response(char** commands){
	if(!loggedin && (strcmp(commands[0], "USER") != 0)){
		sendMsg("Please log in first\n\r");
		return;
	}

	if(strcmp(commands[0], "USER") == 0){
		user(commands[1]);
	}
	else if(strcmp(commands[0], "TYPE") == 0){
		type(commands[1]);
	}
	else if(strcmp(commands[0], "MODE") == 0){
		switchmode(commands[1]);
	}
	else if(strcmp(commands[0], "STRU") == 0){
		switchstru(commands[1]);
	}
	else if(strcmp(commands[0], "CWD") == 0){
		cdinto(commands[1]);
	}
	else if(strcmp(commands[0], "CDUP") == 0){
		cdto_parent();
	}
	else if(strcmp(commands[0], "PASV") == 0){
		sendMsg("227 Enter PASV mode.\n\r");
	}
	else if (strcmp(commands[0], "PORT") == 0){
		printf("The port is %s\n\r", commands[1]);
	}
	else{
		invalid();
	}
}