/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace/workspace_command.c
 *
 * PURPOSE:
 *   Implement the workspace command behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Workspace Command | MIT */
#include "umicom/project/workspace/workspace_command.h"
#include "internal.h"
#include <string.h>
/*
 * Initialise project workspace workspace command from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_project_workspace_workspace_command_init(UmiProjectWorkspaceWorkspaceCommand *value,const char *id,const char *topic,const char *payload) {
    UmiStatus s;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL||id==NULL||topic==NULL||payload==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(value,0,sizeof(*value));
    s=umi_pw_copy(value->id,sizeof(value->id),id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(s!=UMI_STATUS_OK)return s;
    s=umi_pw_copy(value->topic,sizeof(value->topic),topic);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(s!=UMI_STATUS_OK)return s;
    s=umi_pw_copy(value->payload,sizeof(value->payload),payload);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(s!=UMI_STATUS_OK)return s;
    value->sequence=1U;
    return umi_project_workspace_workspace_command_validate(value);
}
/*
 * Check that project workspace workspace command satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_project_workspace_workspace_command_validate(const UmiProjectWorkspaceWorkspaceCommand *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL||value->id[0]=='\0'||value->topic[0]=='\0'||value->sequence==0U)return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
