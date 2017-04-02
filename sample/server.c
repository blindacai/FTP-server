/*
** server.c -- a stream socket server demo
*/

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

#define PORT "3500"  // the port users will be connecting to

#define BACKLOG 10	 // how many pending connections queue will hold

char buf[512];

void sigchld_handler(int s)
{
	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;

	while(waitpid(-1, NULL, WNOHANG) > 0);

	errno = saved_errno;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


void sendError(int new_fd){
	send(new_fd, "500\n", 3+1, 0);
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
		printf("offset is %s\n", *offset);
		if(checkForNewLine(*offset)){
			break;
		}
	}
    return offset - str_array + 1;
}


int main(void)
{
	int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	printf("server: waiting for connections...\n");

	while(1) {  // main accept() loop
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);        // argv: original file descripter, pointer to the address of the client, size of the struct
		if (new_fd == -1) {
			perror("accept");  // should be 'not accept'?
			continue;     // back to the beginning of outer while, waiting for connection
		}

		inet_ntop(their_addr.ss_family,
			get_in_addr((struct sockaddr *)&their_addr),
			s, sizeof s);
		printf("server: got connection from %s\n", s);

		// once connection is set up, send 220
		send(new_fd, "220\n", 3+1, 0);
		
		// inner while loop here to keep waiting for client's commands until quit

		// need a receive function somewhere in the loop

		if (!fork()) { // this is the child process
			close(sockfd); // child doesn't need the listener
			if (send(new_fd, "Hello, world!\n", 13+1, 0) == -1)            // argv: file descripter, message, size of the message, flag(should always be zero)
				perror("send");
			close(new_fd);
			exit(0);
		}

		// note that size of buf is 512
		while(recv(new_fd, buf, sizeof(buf), 0) != -1){
			
			char* piece = strtok(buf, " ");

			// parse the command
			char* commands[3];
			int i = 0;
			while(piece && (i < 3)){
				commands[i] = piece;
				printf("this piece is %s\n", commands[i]);
				piece = strtok(NULL, " ");
				i++;
			}

			if(strncmp(commands[0], "quit", 4) == 0){
				send(new_fd, "221 Goodbye\n", 11+1, 0);
				break;
			}

			printf("array length: %d\n", arr_len(commands));

			memset(buf, 0, strlen(buf));
			// no need to reset commands; it's released from stack once out of while loop
		}

		close(new_fd);  // parent doesn't need this

		continue;
	}

	return 0;
}