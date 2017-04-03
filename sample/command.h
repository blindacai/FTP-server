#ifndef __COMMAND_H__
#define __COMMAND_H__

void sendMsg(char* string);
bool checkForNewLine(char* str);
int arr_len(char** str_array);

void resetLogin();
void set_newfd(int newfd);
void set_rootdir(char* root);

void remove_endofline(char** commands);
void response(char** commands);

void printfCommands(char** commands);
void printfString(char* string);
#endif