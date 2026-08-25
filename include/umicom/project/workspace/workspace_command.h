/* Umicom Framework | Workspace Command | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_PROJECT_WORKSPACE_WORKSPACE_COMMAND_H
#define UMICOM_PROJECT_WORKSPACE_WORKSPACE_COMMAND_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiProjectWorkspaceWorkspaceCommand {
        char id[UMI_PROJECT_WORKSPACE_ID_CAPACITY];
        char topic[UMI_PROJECT_WORKSPACE_ID_CAPACITY];
        char payload[UMI_PROJECT_WORKSPACE_TEXT_CAPACITY];
        uint64_t sequence;
    }
    UmiProjectWorkspaceWorkspaceCommand;
    UmiStatus umi_project_workspace_workspace_command_init(UmiProjectWorkspaceWorkspaceCommand *value,const char *id,const char *topic,const char *payload);
    UmiStatus umi_project_workspace_workspace_command_validate(const UmiProjectWorkspaceWorkspaceCommand *value);
#ifdef __cplusplus
}
#endif
#endif
