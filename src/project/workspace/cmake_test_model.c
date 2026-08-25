/* Umicom Framework | Cmake Test Model | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/project/workspace/cmake_test_model.h"
#include <string.h>
UmiStatus umi_project_workspace_cmake_test_model_init(UmiProjectWorkspaceCmakeTestModel *value,const char *id) {
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(value,0,sizeof(*value));
    return umi_project_workspace_named_state_init(&value->base,id);
}
UmiStatus umi_project_workspace_cmake_test_model_validate(const UmiProjectWorkspaceCmakeTestModel *value) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_validate(&value->base);
}
UmiStatus umi_project_workspace_cmake_test_model_set_name(UmiProjectWorkspaceCmakeTestModel *value,const char *name) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_name(&value->base,name);
}
UmiStatus umi_project_workspace_cmake_test_model_set_detail(UmiProjectWorkspaceCmakeTestModel *value,const char *detail) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_detail(&value->base,detail);
}
UmiStatus umi_project_workspace_cmake_test_model_set_state(UmiProjectWorkspaceCmakeTestModel *value,UmiProjectWorkspaceState state) {
    return value==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_project_workspace_named_state_set_state(&value->base,state);
}
void umi_project_workspace_cmake_test_model_set_metric(UmiProjectWorkspaceCmakeTestModel *value,uint64_t metric) {
    if(value!=NULL) {
        value->metric=metric;
        value->base.revision+=1U;
    }
}
bool umi_project_workspace_cmake_test_model_same_identity(const UmiProjectWorkspaceCmakeTestModel *left,const UmiProjectWorkspaceCmakeTestModel *right) {
    return left!=NULL&&right!=NULL&&umi_project_workspace_named_state_same_identity(&left->base,&right->base);
}
