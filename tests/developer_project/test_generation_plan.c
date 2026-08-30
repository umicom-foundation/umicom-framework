/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_generation_plan.c
 *
 * PURPOSE:
 *   Verify a complete built-in project template renders into a reviewable plan.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/developer_project/generation_plan.h"
#include "umicom/developer_project/generation_request.h"
#include "umicom/developer_project/templates/c23_console.h"

int main(void)
{
    UmiDeveloperProjectGenerationRequest request;
    UmiDeveloperProjectVariableSet variables;
    UmiDeveloperProjectGenerationPlan *plan;

    plan = (UmiDeveloperProjectGenerationPlan *)calloc(1U, sizeof(*plan));
    assert(plan != NULL);

    umi_developer_project_generation_request_init(&request);
    (void)strcpy(request.template_id, "developer.template.c23-console");
    (void)strcpy(request.application_name, "Example");
    (void)strcpy(request.application_id, "org.umicom.example");
    (void)strcpy(request.repository_name, "umicom-example");
    (void)strcpy(request.target_name, "umicom_example");
    (void)strcpy(request.project_root, "C:/work/example");

    assert(umi_developer_project_generation_request_variables(
        &request, &variables) == UMI_STATUS_OK);
    assert(umi_developer_project_generation_plan_build(
        umi_developer_project_template_c23_console(),
        &variables,
        request.project_root,
        plan) == UMI_STATUS_OK);
    assert(plan->file_count >= 5U);
    assert(strcmp(plan->files[0].relative_path, "CMakeLists.txt") == 0);

    free(plan);
    return 0;
}
