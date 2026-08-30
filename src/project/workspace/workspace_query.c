/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace/workspace_query.c
 *
 * PURPOSE:
 *   Implement the workspace query behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Workspace Query | MIT */
#include "umicom/project/workspace/workspace_query.h"
#include "internal.h"
#include <string.h>
UmiStatus umi_project_workspace_workspace_query_init(UmiProjectWorkspaceWorkspaceQuery *value,const char *id,const char *topic,const char *payload) {
    UmiStatus s;
    if(value==NULL||id==NULL||topic==NULL||payload==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(value,0,sizeof(*value));
    s=umi_pw_copy(value->id,sizeof(value->id),id);
    if(s!=UMI_STATUS_OK)return s;
    s=umi_pw_copy(value->topic,sizeof(value->topic),topic);
    if(s!=UMI_STATUS_OK)return s;
    s=umi_pw_copy(value->payload,sizeof(value->payload),payload);
    if(s!=UMI_STATUS_OK)return s;
    value->sequence=1U;
    return umi_project_workspace_workspace_query_validate(value);
}
UmiStatus umi_project_workspace_workspace_query_validate(const UmiProjectWorkspaceWorkspaceQuery *value) {
    if(value==NULL||value->id[0]=='\0'||value->topic[0]=='\0'||value->sequence==0U)return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
