#include "cmdMgr.h"

struct command *commands;
struct parameter *currentParameter;

int cmdMgrHasCurrentParameter(void) {
    return currentParameter != NULL;
}

int isCommandFile(char *name) {
    int i = !strstartswith(name, ".") && strcontains(name, ".command");
    return i;
}

struct command* cmdMgrReadDirectory(const char *path) {
    DIR *d = opendir(path);
    struct dirent *dir;
    if (d) {
        int cnt = 0;
        while ((dir = readdir(d)) != NULL) {
            if (dir->d_type == DT_REG && isCommandFile(dir->d_name)) cnt++;
        }
        closedir(d);
        struct command *cmds = (struct command*)malloc(sizeof(struct command) * cnt);
        d = opendir(path);
        int i = 0;
        while ((dir = readdir(d)) != NULL) {
            if (dir->d_type == DT_REG && isCommandFile(dir->d_name)) {
                char filepath[MAX_OUTBUT_BUFFER];
                sprintf(filepath, "%s/%s", path, dir->d_name);
                struct command *cmd = commandReadFile(filepath);
                cmds[i] = *cmd;
                i++;
            }
        }
        cmds->numParams = cnt;
        return cmds;
    }
    printf("[ERROR] error reading cmd directory\n");
    return NULL;
}

void replaceStringPlaceholder(char *command, struct command *cmd) {
    for (int i = 0; i < strlen(command); i++) {
        if (command[i] == '$') {
            int upto = i+1;
            while(upto < strlen(command) && command[upto] != ' ') upto++;
            char after[strlen(command)-upto];
            sprintf(after, "%.*s", (int)(strlen(command)-upto), command+upto);
            char paramName[upto-i];
            sprintf(paramName, "%.*s", upto-i, command+i+1);
            strtrim(paramName);
            for (int j = 0; j < cmd->numParams; j++) {
                struct parameter param = cmd->parameters[j];
                if (strequals(param.paramName, paramName) && param.value != NULL) {
                    command[i] = '\0';
                    sprintf(command, "%s%s%s", command, param.value, after);
                    i = 0;
                }
            }
        }
    }
}

void replaceStringOutput(char *command, char *replace) {
    strtrim(replace);
    for (int i = 0; i < strlen(command); i++) {
        if (command[i] == '~') {
            int upto = i+1;
            char after[strlen(command)-upto];
            sprintf(after, "%.*s", (int)(strlen(command)-upto), command+upto);
            char paramName[upto-i];
            sprintf(paramName, "%.*s", upto-i, command+i+1);
            
            command[i] = '\0';
            sprintf(command, "%s%s%s", command, replace, after);
            return;
        }
    }
}

void runBashCommand(struct bashCommand *cmd, char *output) {
    char command[MAX_OUTBUT_BUFFER];
    sprintf(command, "%s", cmd->cmd);
    replaceStringPlaceholder(command, cmd->fromCommand);
    int type = CMD_TYPE_PLAYGROUND;
    if (cmd->isSubmodule)
        type = CMD_TYPE_SUBMODULE;
    runCmd(command, output, MAX_OUTBUT_BUFFER-1, type);
}

void answerWithFormat(struct command *cmd, char *format, char *output) {
    char answer[MAX_OUTBUT_BUFFER];
    sprintf(answer, "%s", format);
    replaceStringPlaceholder(answer, cmd);
    replaceStringOutput(answer, output);
    say(answer);
}

void answerCommand(struct command *cmd, char *output) {
    answerWithFormat(cmd, cmd->answer, output);
}

void reactToCommand(struct command *cmd) {
    printf("[CMD] processing cmd [%s]\n", cmd->cmdName);
    for (int i = 0; i < cmd->numParams; i++) {
        struct parameter param = cmd->parameters[i];
        if (param.required && param.value == NULL) {
            currentParameter = &cmd->parameters[i];
            say(param.question);
            return;
        }
    }
    currentParameter = NULL;
    for (int i = 0; i < cmd->numCommands; i++) {
        struct bashCommand param = cmd->bCommands[i];
        
        char output[MAX_OUTBUT_BUFFER];
        runBashCommand(&param, output);
        answerCommand(cmd, output);
    }
    for (int i = 0; i < cmd->numParams; i++) {
        struct parameter *p = &cmd->parameters[i];
        p->value = NULL;
    }
}

int cmdMgrHandleCommand(char *cmd) {
    if (currentParameter != NULL) {
        if (currentParameter->type == PARAMETER_TYPE_STRING) {
            if (strcontains(cmd, ";")) return 0;
            currentParameter->value = (char*)malloc(strlen(cmd)+1);
            memcpy((char*)currentParameter->value, cmd, strlen(cmd)+1);
        }
        answerWithFormat(currentParameter->fromCommand, currentParameter->answer, NULL);
        reactToCommand(currentParameter->fromCommand);
        return 1;
    }
    
    for (int i = 0; i < commands->numParams; i++) {
        struct command *lcmd = &commands[i];
        for (int j = 0; j < lcmd->numVoices; j++) {
            struct voiceText vText = lcmd->vTexts[j];
            if (strequals(cmd, vText.text)) {
                reactToCommand(&commands[i]);
                return 1;
            }
        }
    }
    
    return 0;
}

void cmdMgrLoadCommands(void) {
    commands = cmdMgrReadDirectory(CMD_PATH);
    printf("[CMD] loaded %d commands\n", commands->numParams);
}