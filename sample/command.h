#ifndef __COMMAND_H__
#define __COMMAND_H__

void sendMsg(int new_fd, char* string);
bool checkForNewLine(char* str);
int arr_len(char** str_array);

void response(int new_fd, char** commands);

void printfCommands(char** commands);
void printfString(char* string);
#endif