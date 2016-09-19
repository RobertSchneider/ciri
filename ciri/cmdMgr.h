#ifndef H_CMDMGR
#define H_CMDMGR

#import "command.h"

void cmdMgrLoadCommands(void);
int cmdMgrHandleCommand(char *cmd);
int cmdMgrHasCurrentParameter(void);

#endif