/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace/project_detection_evidence.c
 *
 * PURPOSE:
 *   Implement the project detection evidence behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Project Detection Evidence | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/project/workspace/project_detection_evidence.h"
#include <string.h>
UmiStatus umi_project_workspace_project_detection_evidence_init(UmiProjectWorkspaceProjectDetectionEvidence *value,const char *id) {
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(value,0,sizeof(*value));
    return umi_project_workspace_named_state_init(&value->base,id);
}
UmiStatus umi_project_workspace_project_detection_evidence_validate(const UmiProjectWorkspaceProjectDetectionEvidence *value) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_validate(&value->base);
}
UmiStatus umi_project_workspace_project_detection_evidence_set_name(UmiProjectWorkspaceProjectDetectionEvidence *value,const char *name) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_name(&value->base,name);
}
UmiStatus umi_project_workspace_project_detection_evidence_set_detail(UmiProjectWorkspaceProjectDetectionEvidence *value,const char *detail) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_detail(&value->base,detail);
}
UmiStatus umi_project_workspace_project_detection_evidence_set_state(UmiProjectWorkspaceProjectDetectionEvidence *value,UmiProjectWorkspaceState state) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_state(&value->base,state);
}
void umi_project_workspace_project_detection_evidence_set_metric(UmiProjectWorkspaceProjectDetectionEvidence *value,uint64_t metric) {
    if(value!=NULL) {
        value->metric=metric;
        value->base.revision+=1U;
    }
}
bool umi_project_workspace_project_detection_evidence_same_identity(const UmiProjectWorkspaceProjectDetectionEvidence *left,const UmiProjectWorkspaceProjectDetectionEvidence *right) {
    return left!=NULL&&right!=NULL&&umi_project_workspace_named_state_same_identity(&left->base,&right->base);
}
