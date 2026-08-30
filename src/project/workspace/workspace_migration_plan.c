/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace/workspace_migration_plan.c
 *
 * PURPOSE:
 *   Implement the workspace migration plan behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Workspace Migration Plan | MIT */
#include "umicom/project/workspace/workspace_migration_plan.h"
#include "internal.h"
#include <string.h>
void umi_project_workspace_workspace_migration_plan_init(UmiProjectWorkspaceWorkspaceMigrationPlan *value) {
    if(value!=NULL) {
        (void)memset(value,0,sizeof(*value));
        value->revision=1U;
    }
}
UmiStatus umi_project_workspace_workspace_migration_plan_add(UmiProjectWorkspaceWorkspaceMigrationPlan *value,const char *id,const char *action,bool required) {
    UmiProjectWorkspaceWorkspaceMigrationPlanStep *s;
    size_t i;
    UmiStatus st;
    if(value==NULL||id==NULL||action==NULL||id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    if(value->count>=UMI_PROJECT_WORKSPACE_MEDIUM_CAPACITY)return UMI_STATUS_CAPACITY_EXCEEDED;
    for(i=0U;i<value->count;++i)if(strcmp(value->steps[i].id,id)==0)return UMI_STATUS_ALREADY_EXISTS;
    s=&value->steps[value->count];
    (void)memset(s,0,sizeof(*s));
    st=umi_pw_copy(s->id,sizeof(s->id),id);
    if(st!=UMI_STATUS_OK)return st;
    st=umi_pw_copy(s->action,sizeof(s->action),action);
    if(st!=UMI_STATUS_OK)return st;
    s->required=required;
    value->count+=1U;
    value->revision+=1U;
    return UMI_STATUS_OK;
}
UmiStatus umi_project_workspace_workspace_migration_plan_complete(UmiProjectWorkspaceWorkspaceMigrationPlan *value,const char *id) {
    size_t i;
    if(value==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    for(i=0U;i<value->count;++i)if(strcmp(value->steps[i].id,id)==0) {
        if(!value->steps[i].completed) {
            value->steps[i].completed=true;
            value->completed_count+=1U;
            value->revision+=1U;
        }
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_NOT_FOUND;
}
bool umi_project_workspace_workspace_migration_plan_ready(const UmiProjectWorkspaceWorkspaceMigrationPlan *value) {
    size_t i;
    if(value==NULL)return false;
    for(i=0U;i<value->count;++i)if(value->steps[i].required&&!value->steps[i].completed)return false;
    return true;
}
