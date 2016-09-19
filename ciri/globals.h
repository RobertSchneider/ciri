#ifndef H_GLOBALS
#define H_GLOBALS

#include <string.h>

#define MAX_OUTBUT_BUFFER 1024

#define PYTHON_PATH "/usr/bin/python"
#define CMD_PATH "cmds/"
#define PLAYGROUND_PATH "playground/"
#define SUBMODULE_PATH "submodules/"

#define CMD_TYPE_PLAYGROUND 1
#define CMD_TYPE_SUBMODULE 2

#define LOG_INFO(x) printf("[INFO] "); printf(x); printf("\n");
#define LOG_CMD(x) printf("[CMD] "); printf(x); printf("\n");

//user defines
#define VARIABLE_NAME "bob"
#define CMD_PREFIX "Siri"
#define SAY_COMMAND "sh say.sh"

//lib
int strcontains(char *s1, char *s2);
int strequals(char *s1, char *s2);
int strstartswith(char *s1, char *s2);
void strtrim(char *s);

void say(const char *text);
int runCmd(const char *cmd, char *output, int len, int type);

#endif