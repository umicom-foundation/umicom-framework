/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_generation_request.c
 *
 * PURPOSE:
 *   Verify standard New Project variables and portable target identifiers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/developer_project/generation_request.h"

int main(void)
{
    UmiDeveloperProjectGenerationRequest request;
    UmiDeveloperProjectVariableSet variables;

    umi_developer_project_generation_request_init(&request);
    (void)strcpy(request.template_id, "developer.template.c23-console");
    (void)strcpy(request.application_name, "Example");
    (void)strcpy(request.application_id, "org.umicom.example");
    (void)strcpy(request.repository_name, "umicom-example");
    (void)strcpy(request.target_name, "umicom_example");
    (void)strcpy(request.project_root, "C:/work/example");

    assert(umi_developer_project_generation_request_validate(
        &request, NULL, 0U) == UMI_STATUS_OK);
    assert(umi_developer_project_generation_request_variables(
        &request, &variables) == UMI_STATUS_OK);
    assert(strcmp(
        umi_developer_project_variable_get(&variables, "TARGET_UPPER"),
        "UMICOM_EXAMPLE") == 0);
    return 0;
}
