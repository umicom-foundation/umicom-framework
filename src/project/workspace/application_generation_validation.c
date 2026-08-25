#include "umicom/project/workspace/application_generation_validation.h"
#include "internal.h"
UmiStatus umi_project_workspace_application_generation_validation_evaluate(const UmiProjectWorkspaceGenerationInput*i,char*out_message,size_t capacity) {
    if(i==NULL||out_message==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    if(i->application_id==NULL||i->application_id[0]=='\0') {
        (void)umi_pw_copy(out_message,capacity,"application id is required");
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if(i->template_id==NULL||i->template_id[0]=='\0'||i->target_root==NULL||i->target_root[0]=='\0') {
        (void)umi_pw_copy(out_message,capacity,"template and target root are required");
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if(i->frontend_count==0U) {
        (void)umi_pw_copy(out_message,capacity,"at least one frontend is required");
        return UMI_STATUS_INVALID_STATE;
    }
    (void)umi_pw_copy(out_message,capacity,"ready");
    return UMI_STATUS_OK;
}
