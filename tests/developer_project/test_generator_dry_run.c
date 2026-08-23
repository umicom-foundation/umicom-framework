/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_generator_dry_run.c
 *
 * PURPOSE:
 *   Verify generation can be previewed without mutating the filesystem.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/developer_project/generation_plan.h"
#include "umicom/developer_project/generation_request.h"
#include "umicom/developer_project/generator.h"
#include "umicom/developer_project/templates/c23_console.h"

int main(void)
{
    UmiDeveloperProjectGenerationRequest request;
    UmiDeveloperProjectVariableSet variables;
    UmiDeveloperProjectGenerationPlan *plan;
    UmiDeveloperProjectGeneratorRequest generator;
    UmiDeveloperProjectGeneratorReport report;

    plan = (UmiDeveloperProjectGenerationPlan *)calloc(1U, sizeof(*plan));
    assert(plan != NULL);

    umi_developer_project_generation_request_init(&request);
    (void)strcpy(request.template_id, "developer.template.c23-console");
    (void)strcpy(request.application_name, "Dry Run");
    (void)strcpy(request.application_id, "org.umicom.dryrun");
    (void)strcpy(request.repository_name, "dry-run");
    (void)strcpy(request.target_name, "dry_run");
    (void)strcpy(request.project_root, "C:/path/that/need/not/exist");

    assert(umi_developer_project_generation_request_variables(
        &request, &variables) == UMI_STATUS_OK);
    assert(umi_developer_project_generation_plan_build(
        umi_developer_project_template_c23_console(),
        &variables,
        request.project_root,
        plan) == UMI_STATUS_OK);

    (void)memset(&generator, 0, sizeof(generator));
    generator.plan = plan;
    generator.dry_run = 1;

    assert(umi_developer_project_generator_apply(
        &generator, &report) == UMI_STATUS_OK);
    assert(report.dry_run == 1);
    assert(report.files_created == plan->file_count);

    free(plan);
    return 0;
}
