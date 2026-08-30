/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/generation_request.h
 *
 * PURPOSE:
 *   Prepare standard project-template variables from New Project wizard input.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_GENERATION_REQUEST_H
#define UMICOM_DEVELOPER_PROJECT_GENERATION_REQUEST_H

#include "umicom/developer_project/variable_set.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDeveloperProjectGenerationRequest {
    char template_id[UMI_DEVELOPER_PROJECT_ID_CAPACITY];
    char application_name[UMI_DEVELOPER_PROJECT_NAME_CAPACITY];
    char application_id[UMI_DEVELOPER_PROJECT_ID_CAPACITY];
    char repository_name[UMI_DEVELOPER_PROJECT_ID_CAPACITY];
    char target_name[UMI_DEVELOPER_PROJECT_ID_CAPACITY];
    char project_root[UMI_DEVELOPER_PROJECT_PATH_CAPACITY];
    int dry_run;
    int overwrite_existing_files;
} UmiDeveloperProjectGenerationRequest;

void umi_developer_project_generation_request_init(
    UmiDeveloperProjectGenerationRequest *request);

UmiStatus umi_developer_project_generation_request_validate(
    const UmiDeveloperProjectGenerationRequest *request,
    char *out_message,
    size_t message_capacity);

UmiStatus umi_developer_project_generation_request_variables(
    const UmiDeveloperProjectGenerationRequest *request,
    UmiDeveloperProjectVariableSet *out_variables);

#ifdef __cplusplus
}
#endif

#endif
