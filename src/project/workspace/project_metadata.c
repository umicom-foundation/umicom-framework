/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace/project_metadata.c
 *
 * PURPOSE:
 *   Implement the project metadata behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Project Metadata | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/project/workspace/project_metadata.h"
#include <string.h>
/*
 * Initialise project workspace project metadata from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_project_workspace_project_metadata_init(UmiProjectWorkspaceProjectMetadata *value,const char *id) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(value,0,sizeof(*value));
    return umi_project_workspace_named_state_init(&value->base,id);
}
/*
 * Check that project workspace project metadata satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_project_workspace_project_metadata_validate(const UmiProjectWorkspaceProjectMetadata *value) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_validate(&value->base);
}
/*
 * Provide the project workspace project metadata set name operation used by this module
 * and its client applications.
 */
UmiStatus umi_project_workspace_project_metadata_set_name(UmiProjectWorkspaceProjectMetadata *value,const char *name) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_name(&value->base,name);
}
/*
 * Provide the project workspace project metadata set detail operation used by this module
 * and its client applications.
 */
UmiStatus umi_project_workspace_project_metadata_set_detail(UmiProjectWorkspaceProjectMetadata *value,const char *detail) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_detail(&value->base,detail);
}
/*
 * Provide the project workspace project metadata set state operation used by this module
 * and its client applications.
 */
UmiStatus umi_project_workspace_project_metadata_set_state(UmiProjectWorkspaceProjectMetadata *value,UmiProjectWorkspaceState state) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_state(&value->base,state);
}
/*
 * Provide the project workspace project metadata set metric operation used by this module
 * and its client applications.
 */
void umi_project_workspace_project_metadata_set_metric(UmiProjectWorkspaceProjectMetadata *value,uint64_t metric) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value!=NULL) {
        value->metric=metric;
        value->base.revision+=1U;
    }
}
/*
 * Provide the project workspace project metadata same identity operation used by this
 * module and its client applications.
 */
bool umi_project_workspace_project_metadata_same_identity(const UmiProjectWorkspaceProjectMetadata *left,const UmiProjectWorkspaceProjectMetadata *right) {
    return left!=NULL&&right!=NULL&&umi_project_workspace_named_state_same_identity(&left->base,&right->base);
}
