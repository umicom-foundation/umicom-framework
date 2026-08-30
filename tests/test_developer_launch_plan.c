/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_developer_launch_plan.c
 *
 * PURPOSE:
 *   Verify project launch profiles become structured run/debug operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/launch_plan.h"

#include <assert.h>
#include <string.h>

int main(void)
{
    UmiDeveloperRuntime *runtime = NULL;
    UmiProjectLaunchProfileSnapshot profile = {0};
    UmiDeveloperLaunchPlanRequest request = {0};
    UmiDeveloperLaunchPlanSnapshot plan;
    UmiDeveloperOperationSnapshot operation;

    assert(umi_developer_runtime_create_owned(&runtime) == UMI_STATUS_OK);
    profile.struct_size = (uint32_t)sizeof(profile);
    profile.api_version = UMI_PROJECT_LAUNCH_PROFILE_API_VERSION;
    strcpy(profile.id, "studio.console");
    strcpy(profile.project_id, "studio");
    strcpy(profile.name, "Studio Console");
    strcpy(profile.program, "umicom-studio-console");
    strcpy(profile.arguments, "--doctor \"workspace one\"");
    strcpy(profile.working_directory, ".");
    strcpy(profile.environment_id, "ucrt64");
    profile.debug = 1;
    assert(umi_project_launch_profile_registry_upsert(
        umi_project_workspace_launch_profile(umi_developer_runtime_projects(runtime)),
        &profile) == UMI_STATUS_OK);

    request.struct_size = (uint32_t)sizeof(request);
    request.api_version = UMI_DEVELOPER_LAUNCH_PLAN_API_VERSION;
    request.launch_profile_id = "studio.console";
    request.debug_mode = -1;
    assert(umi_developer_launch_plan_submit(runtime, &request, &plan) == UMI_STATUS_OK);
    assert(plan.operation_kind == UMI_DEVELOPER_OPERATION_DEBUG);
    assert(umi_developer_pipeline_find(
        umi_developer_runtime_pipeline(runtime), plan.operation_id, &operation)
        == UMI_STATUS_OK);
    assert(operation.argument_count == 2U);
    assert(strcmp(operation.arguments[1], "workspace one") == 0);

    umi_developer_runtime_destroy(runtime);
    return 0;
}
