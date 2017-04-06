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

#include <ifaddrs.h>

#include <regex.h>
#include <sys/types.h>
#include "command.h"

#include "dir.h"
#include "dataServer.h"

int new_fd;
bool loggedin = false;
char root_dir[100];


void set_rootdir(char* root){
	strcpy(root_dir, root);
	printf("root dir: %s\r\n", root_dir);
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
	sendMsg("500 Invalid Command.\r\n");
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
		sendMsg("You are already logged in\r\n");
		return;
	}
	if(username == NULL){
		sendMsg("530 Please provide username\r\n");
		return;
	}

	if(strcmp(username, "cs317") == 0){
		sendMsg("230 Login successful.\r\n");
		loggedin = true;
	}
	else{
		sendMsg("530 This FTP server is cs317 only.\r\n");
	}
}


// process 'type image' command
void type(char* thetype){
	if(thetype == NULL){
		sendMsg("Transfer type not specified\r\n");
		return;
	}

	if(strcmp(thetype, "I") == 0){
		sendMsg("200 Switching to Binary mode.\r\n");
	}
	else if(strcmp(thetype, "A") == 0){
		sendMsg("200 Switching to ASCII mode.\r\n");
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
		fprintf(stderr, "Could not compile regex\r\n"); 
    }

	// execute
	reti = regexec(&regex, dir, 0, NULL, 0);
	return !reti;
}


// process 'cwd local' command
void cdinto(char* dir){
	if(dir == NULL){
		sendMsg("Please provide the directory\r\n");
		return;
	}

	if(notallow_match(dir)){
		sendMsg("550 Not allowed to change directory this way.\r\n");
		listFiles(1, "./");            // for testing
	}
	else{
		if(chdir(dir) == 0){
			sendMsg("250 Directory successfully changed.\r\n");
		}
		else{
			sendMsg("550 Failed to change directory.\r\n");
		}
	
		listFiles(1, "./");            // for testing
	}
}


// process 'MODE' command
void switchmode(char* mode){
	if(mode == NULL){
		sendMsg("Transfer mode not specified\r\n");
	}
	else{
		sendMsg("We only support stream mode, sorry.\r\n");
	}
}


// process 'STRU' command
void switchstru(char* stru){
	if(stru == NULL){
		sendMsg("File structure not specified\r\n");
	}
	else{
		sendMsg("We only support file structure, sorry.\r\n");
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
		sendMsg("550 Cannot change directory. Already in root\r\n");
	}
	else{
		chdir("../");
		sendMsg("250 Directory successfully changed.\r\n");
		listFiles(1, "./");              // for testing
	}
}

// convert to uppercase
void uppercase (char *sPtr) {
    while(*sPtr = toupper(*sPtr)) sPtr++;
}


// create server response
void response(char** commands){
	if(!loggedin && (strcmp(commands[0], "USER") != 0)){
		sendMsg("Please log in first\r\n");
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
		// get current IP address: code section taken from 
		// http://stackoverflow.com/questions/4139405/how-can-i-get-to-know-the-ip-address-for-interfaces-in-c
		struct ifaddrs *ifap, *ifa;
    	struct sockaddr_in *sa;
    	char *addr;

    	getifaddrs (&ifap);
    	for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
        	if (ifa->ifa_addr->sa_family==AF_INET) {
          	  	sa = (struct sockaddr_in *) ifa->ifa_addr;
          	  	addr = inet_ntoa(sa->sin_addr);
          	 	printf("Interface: %s\tAddress: %s\n", ifa->ifa_name, addr);
          	 	if(strcmp(addr, "127.0.0.1") != 0){
          	 		break;
          	 	}
        	}
   		 }
    	freeifaddrs(ifap);
    	// end of code taken section

    	printf("%s\r\n",addr);

		srand(time(NULL));

		char serverIPPort[512];
		int portA = 100 + (rand() % 64);
		int portB = rand() % 256;
		int port = (portA * 256) + portB;

		char portAsChar[512];
		sprintf(portAsChar, "%d", port);
		printf("%s\r\n", portAsChar);

		printf("%d and %d and %d\r\n", portA, portB, port);

		char* IPPiece = strtok(addr, ".");

		char* IPChunk[4] = {NULL, NULL, NULL, NULL};  // should it be dynamic?
		int i;
		for(i = 0; i < 4; i++){
			IPChunk[i] = IPPiece;
			printf("piece is %s\r\n", IPPiece);
			IPPiece = strtok(NULL, ".");
		}
		printf("made it\n");

		// really unncessary - shoudl remove 

		char IP1[32];
		char IP2[32];
		char IP3[32];
		char IP4[32];
		char port1[256];
		char port2[256];

		sprintf(IP1, "%s", IPChunk[0]);
		printf("piece is %s\r\n", IP1);
		sprintf(IP2, "%s", IPChunk[1]);
		printf("piece is %s\r\n", IP2);

		sprintf(IP3, "%s", IPChunk[2]);
		printf("piece is %s\r\n", IP3);
		sprintf(IP4, "%s", IPChunk[3]);
		printf("piece is %s\r\n", IP4);



		sprintf(port1, "%d", portA);
		printf("piece is %s\r\n", port1);
		sprintf(port2, "%d", portB);
		printf("piece is %s\r\n", port2);

		// remove above... use it for comments i guess


		char myString[1024];
		sprintf(myString, "227 Entering Passive Mode (%s,%s,%s,%s,%d,%d)\r\n", IPChunk[0], IPChunk[1], IPChunk[2], IPChunk[3], portA, portB);
		printf("still here \n");

		//snprintf(myString, sizeof myString, "227 Entering Pasive Mode (198,162,33,14,%s,%s)\r\n", portA, portB);

		listenOnDataConnect(portAsChar);
		sendMsg(myString);
	}
	else if(strcmp(commands[0], "NLST") == 0){
		sendMsg("150 Here comes the list\r\n");
		// switch to data connection
		int data_fd = acceptDataConnect();

		// back to control
		sendMsg("226 List transfer done\r\n");

		// close data connection
		close(data_fd);
		close(getDataSocket());
	}
	else if(strcmp(commands[0], "RETR") == 0){

	}
	
	else{
		invalid();
	}
}