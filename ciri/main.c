#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "globals.h"
#include "command.h"
#include "main.h"
#include "cmdMgr.h"

#include "server.h"

int nextOneFree = 0;

int runCmd(const char *cmd, char *output, int len, int type) {
    char cmdC[MAX_OUTBUT_BUFFER];
    if (type == CMD_TYPE_PLAYGROUND)
        sprintf(cmdC, "cd " PLAYGROUND_PATH ";%s", cmd);
    if (type == CMD_TYPE_SUBMODULE)
        sprintf(cmdC, "cd " SUBMODULE_PATH ";%s", cmd);
    printf("[CMD] %s\n", cmdC);
    FILE *fp;
    fp = popen(cmdC, "r");
    if (fp == NULL) {
        return 1;
    }
    fgets(output, len-1, fp);
    return 0;
}

void say(const char *text) {
    char buffer[MAX_OUTBUT_BUFFER];
    char cmd[MAX_OUTBUT_BUFFER];
    //sprintf(cmd, "say [[inpt PHON]]%s[[inpt TEXT]]", text);
    sprintf(cmd, SAY_COMMAND " \"%s\"", text);
    runCmd(cmd, buffer, MAX_OUTBUT_BUFFER, CMD_TYPE_SUBMODULE);
}

void handleCommand(char *cmd) {
    if (!strstartswith(cmd, CMD_PREFIX) && !nextOneFree && !cmdMgrHasCurrentParameter()) {
        printf("[INFO] ingoring [%s]\n", cmd);
        return;
    }else if (strequals(cmd, CMD_PREFIX)) {
        nextOneFree = 1;
        say("yes?");
        return;
    }
    if (strstartswith(cmd, CMD_PREFIX))
        cmd+=strlen(CMD_PREFIX)+1;
    nextOneFree = 0;
    if (!cmdMgrHandleCommand(cmd)) {
        say("I did not understand:");
        say((const char*)cmd);
    }
}

void *listenThread(void* arg) {
    /*while(1) {
        char buffer[MAX_OUTBUT_BUFFER];
        runCmd("python listen_mic.py", buffer, MAX_OUTBUT_BUFFER, CMD_TYPE_SUBMODULE);
        int len = (int)strlen(buffer);
        for (int i = 0; i < len; i++) {
            if (buffer[i] == '\'') buffer[i] = ' ';
        }
        printf("[CMD] recognized %s\n", buffer);
        handleCommand(buffer);
        buffer[0] = '\0';
    }*/
	startServer();
}

int main(void) {
    LOG_INFO("**CIRI STARTED**");

    cmdMgrLoadCommands();
    
    say("hey " VARIABLE_NAME " how can I help you?");
    
    pthread_t pthListen;
    pthread_create(&pthListen, NULL, listenThread, NULL);
    
    //say("you have 0 new mails");
    pthread_join(pthListen, NULL);
    
    return 0;
}
