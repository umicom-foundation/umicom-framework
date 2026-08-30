/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace/project_variant.c
 *
 * PURPOSE:
 *   Implement the project variant behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Project Variant | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/project/workspace/project_variant.h"
#include <string.h>
UmiStatus umi_project_workspace_project_variant_init(UmiProjectWorkspaceProjectVariant *value,const char *id) {
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(value,0,sizeof(*value));
    return umi_project_workspace_named_state_init(&value->base,id);
}
UmiStatus umi_project_workspace_project_variant_validate(const UmiProjectWorkspaceProjectVariant *value) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_validate(&value->base);
}
UmiStatus umi_project_workspace_project_variant_set_name(UmiProjectWorkspaceProjectVariant *value,const char *name) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_name(&value->base,name);
}
UmiStatus umi_project_workspace_project_variant_set_detail(UmiProjectWorkspaceProjectVariant *value,const char *detail) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_detail(&value->base,detail);
}
UmiStatus umi_project_workspace_project_variant_set_state(UmiProjectWorkspaceProjectVariant *value,UmiProjectWorkspaceState state) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_state(&value->base,state);
}
void umi_project_workspace_project_variant_set_metric(UmiProjectWorkspaceProjectVariant *value,uint64_t metric) {
    if(value!=NULL) {
        value->metric=metric;
        value->base.revision+=1U;
    }
}
bool umi_project_workspace_project_variant_same_identity(const UmiProjectWorkspaceProjectVariant *left,const UmiProjectWorkspaceProjectVariant *right) {
    return left!=NULL&&right!=NULL&&umi_project_workspace_named_state_same_identity(&left->base,&right->base);
}
