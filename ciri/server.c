#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
#include <string.h>

#include "sha1.h"
#include "base64.h"
#include "ws_protocol.h"
#include "server.h"

#define CLIENTPORT 48300
#define BROADPORT 6666
#define MAXBUFLEN 5120

//globals
struct User *user;

//CODE

void error(char *msg)
{
	perror(&msg[0]);
	//exit(1);
}
void logStr(char *str)
{
	//no log because of daemon
	printf("%s\n", str);
}


int startServer()
{
	/*//daemon
	pid_t pid, sid;
	pid = fork();
	if (pid != 0) {
		exit(EXIT_FAILURE);
	}
	sid = setsid();
	if (sid < 0) {
		exit(EXIT_FAILURE);
	}*/

	//listen for incomming connections
	listenSocketServer();
	return 0;
}

char *packFrame(char *msg)
{
	int len = 1 + strlen(msg) + 1 + 4;
	int pos = 2;
	if (strlen(msg) >= 126 && strlen(msg) <= 65535) { len += 3; pos += 2; }

	char *frame = malloc(len + 1);
	frame[0] = (char)129;

	if (strlen(msg) < 125)
	{
		frame[1] = (int)strlen(msg);
	}
	else if (strlen(msg) >= 126 && strlen(msg) <= 65535)
	{
		frame[1] = (char)126;
		frame[2] = (char)((strlen(msg) >> 8) & 255);
		frame[3] = (char)(strlen(msg) & 255);
	}
	strcpy(frame + pos, msg);

	frame[len - 3] = '\r';
	frame[len - 2] = '\n';
	frame[len - 1] = '\0';
	return frame;
}
void writeFrame(int sock, char *s)
{
	char *s2;
	s2 = packFrame(s);
	write(sock, s2, strlen(s2));
	free(s2);
}

void closeSocket(int socket)
{
	//remove socket
	close(socket);
	logStr("socket closed");
}

void replace(char* c, int len, char cfrom, char cto)
{
	for (int i = 0; i < len; ++i)
	{
		if (c[i] == cfrom) c[i] = cto;
	}
}

//MAIN
void* clientMain(void *arg)
{
	char buffer[MAXBUFLEN];
	int n;
	int socket = *(int*)arg;

	while (1)
	{
		memset((char *)&buffer, '\0', MAXBUFLEN);
		n = read(socket, &buffer, MAXBUFLEN - 1);
		if (n == 0 || (int)(buffer[0] & 0x0F) == 0)
		{
			closeSocket(socket);
			break;
		}
		int opcode = (int)(buffer[0] & 0x0F);
		//text
		if (opcode == 1)
		{
			int len = 0;
			uint8_t *outBuffer = parse((const uint8_t*)buffer, n, &len);
			if (outBuffer == NULL)
			{
				logStr("ERROR reading / parsing frame failed");
				continue;
			};
			if (n < 0) logStr("ERROR reading socket failed");
			replace((char*)outBuffer, len, '\\', ' ');
			replace((char*)outBuffer, len, '<', ' ');
			replace((char*)outBuffer, len, '>', ' ');

			json_object *json = json_tokener_parse((char*)outBuffer);
			char *msg;
			json_object_object_foreach(json, key, val)
			{
				const char *v = json_object_get_string(val);
				if (strcmp(key, "msg") == 0) {
					msg = malloc(strlen(v) + 1);
					strcpy(msg, v);
					msg[strlen(v)] = '\0';
				}
			}
			json_object_put(json);

			printf("%d\n", strlen(msg));
			handleCommand(msg);
			logStr(msg);

			free(msg);
			free(outBuffer);

			/*struct BccMessage *bccMsg = createMessage(nick, ip, msg);
			if (bccMsg == NULL) { logStr("ERROR parsing msg"); continue; }

			logStr("encoding...");
			char *toSend = encodeMessage(bccMsg);
			broadcastMsg(toSend);
			free(toSend);

			free(nick);
			free(msg);
			free(bccMsg->msg);
			free(bccMsg);
			free(outBuffer);*/
		}
		else if (opcode == 10)
		{
			//pong!
			logStr("PONG! received");
		}
		else if (opcode == 9)
		{
			//ping!
			logStr("PING! received");
			char *pckt = packFrame("PING");
			pckt[0] = (char)0b10001001;
			write(socket, pckt, strlen(pckt));
			free(pckt);
		}
		else if (opcode == 8)
		{
			//close
			write(socket, buffer, (size_t)n);
			closeSocket(socket);
			break;
		}
		else
		{
			//wtf is thaat?
		}
	}
	return NULL;
}

int listenSocketServer()
{
	int serverSock, clientSock;
	struct sockaddr_in serv_addr, cli_addr;
	int n;

	//init socket
	serverSock = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSock < 0) logStr("ERROR socket creation failed");
	memset((char *)&serv_addr, '\0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(CLIENTPORT);

	//bind and listen
	int reuse = -1;
	if (setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0) logStr("ERROR port reusing failed");
	if (bind(serverSock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)  logStr("ERROR socket binding failed");
	listen(serverSock, 5);
	socklen_t clilen = sizeof(cli_addr);

	//accepting new clients
	while (1)
	{
		clientSock = accept(serverSock, (struct sockaddr *) &cli_addr, &clilen);
		if (clientSock < 0)
		{
			logStr("ERROR accepting new socket failed");
			continue;
		}
		logStr("connected");

		//handshake
		handshake(clientSock);

		user = malloc(sizeof(struct User));
		if (user == NULL)
		{
			logStr("error allocating user\n");
			continue;
		}

		user->socket = clientSock;
		strcpy(user->ip, inet_ntoa(cli_addr.sin_addr));
		user->ip[39] = '\0';

		//new thread for every client
		pthread_t pth;
		pthread_create(&pth, NULL, clientMain, &clientSock);
	}

	logStr("closing server\n");
	return 0;
}
