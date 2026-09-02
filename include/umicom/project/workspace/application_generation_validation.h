/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/application_generation_validation.h
 *
 * PURPOSE:
 *   Publish the public application generation validation contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_APPLICATION_GENERATION_VALIDATION_H
#define UMICOM_PROJECT_WORKSPACE_APPLICATION_GENERATION_VALIDATION_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the project workspace generation input data shared with callers of this public
 * contract.
 */
    typedef struct UmiProjectWorkspaceGenerationInput {
        const char*application_id;
        const char*template_id;
        const char*target_root;
        size_t component_count;
        size_t frontend_count;
    }
    UmiProjectWorkspaceGenerationInput;
    UmiStatus umi_project_workspace_application_generation_validation_evaluate(const UmiProjectWorkspaceGenerationInput*input,char*out_message,size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
