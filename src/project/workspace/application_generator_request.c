/* Umicom Framework | Application Generator Request | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/project/workspace/application_generator_request.h"
#include <string.h>
UmiStatus umi_project_workspace_application_generator_request_init(UmiProjectWorkspaceApplicationGeneratorRequest *value,const char *id) {
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(value,0,sizeof(*value));
    return umi_project_workspace_named_state_init(&value->base,id);
}
UmiStatus umi_project_workspace_application_generator_request_validate(const UmiProjectWorkspaceApplicationGeneratorRequest *value) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_validate(&value->base);
}
UmiStatus umi_project_workspace_application_generator_request_set_name(UmiProjectWorkspaceApplicationGeneratorRequest *value,const char *name) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_name(&value->base,name);
}
UmiStatus umi_project_workspace_application_generator_request_set_detail(UmiProjectWorkspaceApplicationGeneratorRequest *value,const char *detail) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_detail(&value->base,detail);
}
UmiStatus umi_project_workspace_application_generator_request_set_state(UmiProjectWorkspaceApplicationGeneratorRequest *value,UmiProjectWorkspaceState state) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_state(&value->base,state);
}
void umi_project_workspace_application_generator_request_set_metric(UmiProjectWorkspaceApplicationGeneratorRequest *value,uint64_t metric) {
    if(value!=NULL) {
        value->metric=metric;
        value->base.revision+=1U;
    }
}
bool umi_project_workspace_application_generator_request_same_identity(const UmiProjectWorkspaceApplicationGeneratorRequest *left,const UmiProjectWorkspaceApplicationGeneratorRequest *right) {
    return left!=NULL&&right!=NULL&&umi_project_workspace_named_state_same_identity(&left->base,&right->base);
}
