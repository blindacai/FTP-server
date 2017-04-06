#ifndef __DATASERVER_H__
#define __DATASERVER_H__

int listenOnDataConnect(char* port);
int acceptDataConnect();
int getDataSocket();

void setListfile();
void setFilename(char* name);
#endif