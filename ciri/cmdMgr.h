#ifndef H_CMDMGR
#define H_CMDMGR

#include "command.h"

void cmdMgrLoadCommands(void);
int cmdMgrHandleCommand(char *cmd);
int cmdMgrHasCurrentParameter(void);

#endif
