#ifndef H_COMMAND
#define H_COMMAND

#include <json-c/json.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include <dirent.h> 
#include "globals.h"

#define PARAMETER_TYPE_STRING 1

struct voiceText {
    char *text;
};
struct bashCommand {
    char *cmd;
    int isSubmodule;
    struct command *fromCommand;
};

struct parameter {
    char *paramName;
    char *question;
    void *value;
    int required;
    int type;
    char *answer;
    struct command *fromCommand;
};

struct command {
    char *cmdName;
    struct voiceText *vTexts;
    int numVoices;
    struct bashCommand *bCommands;
    int numCommands;
    struct parameter *parameters;
    int numParams;
    char *answer;
};

struct command* commandReadDirectory(const char *path);
struct command* commandReadFile(const char *path);

#endif