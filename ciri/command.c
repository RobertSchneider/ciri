#include "command.h"

void readJsonIntoString(json_object *jobj, char **dest) {
    const char *s = json_object_get_string(jobj);
    *dest = (char*)malloc(strlen(s)+1);
    memcpy(*dest, s, strlen(s)+1);
}

void readJsonIntoInt(json_object *jobj, int *dest) {
    int i = json_object_get_int(jobj);
    *dest = i;
}

void readFile_readNameAndAnswer(json_object *jobj, struct command *newCmd) {
    json_object *jname;
    json_object_object_get_ex(jobj, "name", &jname);
    readJsonIntoString(jname, &newCmd->cmdName);
    
    json_object *janswer;
    json_object_object_get_ex(jobj, "answer", &janswer);
    readJsonIntoString(janswer, &newCmd->answer);
}

void readFile_readVoiceTexts(json_object *jobj, struct command *newCmd) {
    json_object *jvtexts;
    json_object_object_get_ex(jobj, "voiceTexts", &jvtexts);
    int vtextsLen = json_object_array_length(jvtexts);
    struct voiceText *vTexts = (struct voiceText*)malloc(sizeof(struct voiceText) * vtextsLen);
    newCmd->vTexts = vTexts;
    newCmd->numVoices = vtextsLen;
    for (int i = 0; i < vtextsLen; i++) {
        struct json_object *jvtext = json_object_array_get_idx(jvtexts, i);
        struct voiceText *vText = (struct voiceText*)malloc(sizeof(struct voiceText));
        readJsonIntoString(jvtext, &vText->text);
        vTexts[i] = *vText;
    }
}

void readFile_readBashCmds(json_object *jobj, struct command *newCmd) {
    json_object *jbCmds;
    json_object_object_get_ex(jobj, "bashCommands", &jbCmds);
    int bCmdsLen = json_object_array_length(jbCmds);
    struct bashCommand *bCmds = (struct bashCommand*)malloc(sizeof(struct bashCommand) * bCmdsLen);
    newCmd->bCommands = bCmds;
    newCmd->numCommands = bCmdsLen;
    for (int i = 0; i < bCmdsLen; i++) {
        struct json_object *jbcmd = json_object_array_get_idx(jbCmds, i);
        struct bashCommand *bCmd = (struct bashCommand*)malloc(sizeof(struct bashCommand));
        
        json_object *jcmd;
        json_object_object_get_ex(jbcmd, "cmd", &jcmd);
        readJsonIntoString(jcmd, &bCmd->cmd);
        
        json_object *jsubmod;
        json_object_object_get_ex(jbcmd, "isSubmodule", &jsubmod);
        readJsonIntoInt(jsubmod, &bCmd->isSubmodule);
        
        bCmd->fromCommand = newCmd;
        bCmds[i] = *bCmd;
    }
}

void readFile_readParameter(json_object *jobj, struct parameter *param) {
    json_object *jname;
    json_object *jquestion;
    json_object *jrequired;
    json_object *jtype;
    json_object *janswer;
    json_object_object_get_ex(jobj, "name", &jname);
    json_object_object_get_ex(jobj, "question", &jquestion);
    json_object_object_get_ex(jobj, "answer", &janswer);
    json_object_object_get_ex(jobj, "required", &jrequired);
    json_object_object_get_ex(jobj, "type", &jtype);
    
    readJsonIntoString(jname, &param->paramName);
    readJsonIntoString(jquestion, &param->question);
    readJsonIntoString(janswer, &param->answer);
    readJsonIntoInt(jrequired, &param->required);
    readJsonIntoInt(jtype, &param->type);
}

void readFile_readParameters(json_object *jobj, struct command *newCmd) {
    json_object *jparams;
    json_object_object_get_ex(jobj, "parameters", &jparams);
    int paramsLen = json_object_array_length(jparams);
    struct parameter *params = (struct parameter*)malloc(sizeof(struct parameter) * paramsLen);
    newCmd->parameters = params;
    newCmd->numParams = paramsLen;
    for (int i = 0; i < paramsLen; i++) {
        struct json_object *jparam = json_object_array_get_idx(jparams, i);
        struct parameter *param = (struct parameter*)malloc(sizeof(struct parameter));
        readFile_readParameter(jparam, param);
        param->fromCommand = newCmd;
        params[i] = *param;
    }
}

struct command* commandReadFile(const char *path) {
    struct command *dest = (struct command*)malloc(sizeof(struct command));
    
    FILE *fp = fopen(path, "r");
    
    fseek(fp, 0, SEEK_END);
    int fsize = (int)ftell(fp) + 1;
    fseek(fp, 0, SEEK_SET);
    
    char *buffer = calloc(fsize, sizeof(char));
    fread(buffer, sizeof(char), fsize, fp);
    fclose(fp);
    
    json_object *jobj = json_tokener_parse(buffer);
    
    readFile_readNameAndAnswer(jobj, dest);
    readFile_readVoiceTexts(jobj, dest);
    readFile_readParameters(jobj, dest);
    readFile_readBashCmds(jobj, dest);
    
    json_object_put(jobj);
    free(buffer);
    return dest;
}