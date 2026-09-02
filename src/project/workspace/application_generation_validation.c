/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace/application_generation_validation.c
 *
 * PURPOSE:
 *   Implement the application generation validation behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/application_generation_validation.h"
#include "internal.h"
/*
 * Provide the project workspace application generation validation evaluate operation used
 * by this module and its client applications.
 */
UmiStatus umi_project_workspace_application_generation_validation_evaluate(const UmiProjectWorkspaceGenerationInput*i,char*out_message,size_t capacity) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(i==NULL||out_message==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(i->application_id==NULL||i->application_id[0]=='\0') {
        (void)umi_pw_copy(out_message,capacity,"application id is required");
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(i->template_id==NULL||i->template_id[0]=='\0'||i->target_root==NULL||i->target_root[0]=='\0') {
        (void)umi_pw_copy(out_message,capacity,"template and target root are required");
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if(i->frontend_count==0U) {
        (void)umi_pw_copy(out_message,capacity,"at least one frontend is required");
        return UMI_STATUS_INVALID_STATE;
    }
    (void)umi_pw_copy(out_message,capacity,"ready");
    return UMI_STATUS_OK;
}
