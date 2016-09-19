#ifndef SERVER_H
#define SERVER_H

#include <json-c/json.h>

struct User
{
	char ip[40];
	int socket;
};

char *packFrame(char *msg);
void closeSocket(int socket);

//methods
void error(char *msg);

//MAIN
int listenSocketServer();
void* clientMain(void *arg);
int startServer();

#endif