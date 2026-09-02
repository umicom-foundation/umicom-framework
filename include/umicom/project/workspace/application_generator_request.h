/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/application_generator_request.h
 *
 * PURPOSE:
 *   Publish the public application generator request contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_APPLICATION_GENERATOR_REQUEST_H
#define UMICOM_PROJECT_WORKSPACE_APPLICATION_GENERATOR_REQUEST_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the project workspace application generator request data shared with callers
 * of this public contract.
 */
    typedef struct UmiProjectWorkspaceApplicationGeneratorRequest {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceApplicationGeneratorRequest;
    UmiStatus umi_project_workspace_application_generator_request_init(UmiProjectWorkspaceApplicationGeneratorRequest *value,const char *id);
    UmiStatus umi_project_workspace_application_generator_request_validate(const UmiProjectWorkspaceApplicationGeneratorRequest *value);
    UmiStatus umi_project_workspace_application_generator_request_set_name(UmiProjectWorkspaceApplicationGeneratorRequest *value,const char *name);
    UmiStatus umi_project_workspace_application_generator_request_set_detail(UmiProjectWorkspaceApplicationGeneratorRequest *value,const char *detail);
    UmiStatus umi_project_workspace_application_generator_request_set_state(UmiProjectWorkspaceApplicationGeneratorRequest *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_application_generator_request_set_metric(UmiProjectWorkspaceApplicationGeneratorRequest *value,uint64_t metric);
    bool umi_project_workspace_application_generator_request_same_identity(const UmiProjectWorkspaceApplicationGeneratorRequest *left,const UmiProjectWorkspaceApplicationGeneratorRequest *right);
#ifdef __cplusplus
}
#endif
#endif
