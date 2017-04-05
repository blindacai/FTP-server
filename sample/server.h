#ifndef __SERVER_H__
#define __SERVER_H__

int listenOnConnect(char* port);
void acceptConnect();
void acceptDataConnect();
#endif