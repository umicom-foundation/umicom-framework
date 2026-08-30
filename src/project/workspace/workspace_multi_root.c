/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace/workspace_multi_root.c
 *
 * PURPOSE:
 *   Implement the workspace multi root behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/workspace_multi_root.h"
#include "internal.h"
#include <string.h>
void umi_project_workspace_workspace_multi_root_init(UmiProjectWorkspaceWorkspaceMultiRoot *value) {
    if(value!=NULL)(void)memset(value,0,sizeof(*value));
}
UmiStatus umi_project_workspace_workspace_multi_root_add_root(UmiProjectWorkspaceWorkspaceMultiRoot *value,const char *path) {
    size_t i;
    UmiStatus s;
    if(value==NULL||path==NULL||path[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    for(i=0U;i<value->count;++i)if(strcmp(value->roots[i],path)==0)return UMI_STATUS_ALREADY_EXISTS;
    if(value->count>=UMI_PROJECT_WORKSPACE_SMALL_CAPACITY)return UMI_STATUS_CAPACITY_EXCEEDED;
    s=umi_pw_copy(value->roots[value->count],sizeof(value->roots[value->count]),path);
    if(s==UMI_STATUS_OK)value->count+=1U;
    return s;
}
bool umi_project_workspace_workspace_multi_root_valid(const UmiProjectWorkspaceWorkspaceMultiRoot *value) {
    return value!=NULL&&value->count>0U;
}
