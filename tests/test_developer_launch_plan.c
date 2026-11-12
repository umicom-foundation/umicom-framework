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
/* Required fixture setup and checks also execute in optimized builds. */
#include "support/assertions.h"
#include "umicom/developer/launch_plan.h"

#include <assert.h>
#include <string.h>

/* A refused review must retain all publication identities, not merely omit
 * the output plan. Use real registry/runtime owners and never start a process. */
static void reviewed_launch_cases(void)
{
    UmiDeveloperRuntime *runtime = NULL;
    assert(umi_developer_runtime_create_owned(&runtime) == UMI_STATUS_OK);
    UmiProjectLaunchProfileRegistry *registry = umi_project_workspace_launch_profile(
        umi_developer_runtime_projects(runtime));
    UmiProjectLaunchProfileSnapshot draft = {0}, expected;
    draft.struct_size = sizeof(draft);
    draft.api_version = UMI_PROJECT_LAUNCH_PROFILE_API_VERSION;
    strcpy(draft.id, "reviewed"); strcpy(draft.program, "original-program");
    strcpy(draft.arguments, "'' 'two words'");
    assert(umi_project_launch_profile_registry_upsert(registry, &draft) == UMI_STATUS_OK);
    assert(umi_project_launch_profile_registry_find(registry, draft.id, &expected) == UMI_STATUS_OK);
    UmiDeveloperLaunchPlanRequest request = {0};
    request.struct_size = sizeof(request);
    request.api_version = UMI_DEVELOPER_LAUNCH_PLAN_API_VERSION;
    request.launch_profile_id = "reviewed";
    request.operation_id = "reviewed.launch";
    request.debug_mode = -1;
    unsigned char scratch[2U * sizeof(expected) + 256U];
    UmiValueArchiveComparison measured;
    assert(umi_developer_launch_plan_compare_profile(runtime, &expected, NULL, 0U, &measured) == UMI_STATUS_OK);
    assert(measured.required_workspace > 0U && measured.required_workspace <= sizeof(scratch));
    UmiDeveloperLaunchPlanSnapshot plan, sentinel;
    memset(&plan, 0x5a, sizeof(plan)); memcpy(&sentinel, &plan, sizeof(plan));

    /* Each case changes a different boundary. Registry edits are intentional;
     * runtime publication must remain unchanged after the refused request. */
    for (unsigned scenario = 0U; scenario < 9U; ++scenario) {
        UmiProjectLaunchProfileSnapshot reviewed = expected;
        UmiDeveloperLaunchPlanRequest attempt = request;
        void *storage = scratch;
        size_t capacity = sizeof(scratch);
        UmiStatus refusal = UMI_STATUS_INVALID_ARGUMENT;
        if (scenario == 0U) storage = NULL;
        if (scenario == 1U) { capacity = measured.required_workspace - 1U; refusal = UMI_STATUS_CAPACITY_EXCEEDED; }
        if (scenario == 2U) attempt.launch_profile_id = "different";
        if (scenario == 3U) attempt.api_version += 1U;
        if (scenario == 4U) { storage = &reviewed; capacity = sizeof(reviewed); }
        if (scenario == 5U) { strcpy(reviewed.program, "changed-program"); refusal = UMI_STATUS_INVALID_STATE; }
        if (scenario == 6U) { reviewed.revision += 1U; refusal = UMI_STATUS_INVALID_STATE; }
        if (scenario == 7U) { storage = &plan; capacity = sizeof(plan); }
        if (scenario == 8U) { strcpy((char *)scratch, "reviewed.launch"); attempt.operation_id = (char *)scratch; }
        UmiDeveloperRuntimeSnapshot before, after;
        assert(umi_developer_runtime_snapshot(runtime, &before) == UMI_STATUS_OK);
        assert(umi_developer_launch_plan_submit_reviewed(runtime, &attempt, &reviewed,
            storage, capacity, &plan) == refusal);
        assert(umi_developer_runtime_snapshot(runtime, &after) == UMI_STATUS_OK);
        assert(before.revision == after.revision);
        assert(before.pipeline.revision == after.pipeline.revision);
        assert(before.pipeline.operation_count == after.pipeline.operation_count);
        assert(before.journal.revision == after.journal.revision);
        assert(before.journal.next_sequence == after.journal.next_sequence);
        assert(memcmp(&plan, &sentinel, sizeof(plan)) == 0);
    }
    /* A changed live profile refuses the earlier captured review. Restoring
     * identical text still has a newer revision and requires another capture. */
    strcpy(draft.program, "changed-program");
    assert(umi_project_launch_profile_registry_upsert(registry, &draft) == UMI_STATUS_OK);
    assert(umi_developer_launch_plan_submit_reviewed(runtime, &request, &expected,
        scratch, sizeof(scratch), &plan) == UMI_STATUS_INVALID_STATE);
    assert(memcmp(&plan, &sentinel, sizeof(plan)) == 0);
    strcpy(draft.program, "original-program");
    assert(umi_project_launch_profile_registry_upsert(registry, &draft) == UMI_STATUS_OK);
    assert(umi_developer_launch_plan_submit_reviewed(runtime, &request, &expected,
        scratch, sizeof(scratch), &plan) == UMI_STATUS_INVALID_STATE);
    assert(memcmp(&plan, &sentinel, sizeof(plan)) == 0);
    UmiDeveloperRuntimeSnapshot refused;
    assert(umi_developer_runtime_snapshot(runtime, &refused) == UMI_STATUS_OK);
    assert(refused.pipeline.operation_count == 0U && refused.journal.entry_count == 0U);
    assert(umi_project_launch_profile_registry_find(registry, draft.id, &expected) == UMI_STATUS_OK);
    assert(umi_developer_launch_plan_submit_reviewed(runtime, &request, &expected,
        scratch, sizeof(scratch), &plan) == UMI_STATUS_OK);
    UmiDeveloperOperationSnapshot operation;
    assert(umi_developer_pipeline_find(umi_developer_runtime_pipeline(runtime),
        plan.operation_id, &operation) == UMI_STATUS_OK);
    assert(strcmp(operation.program, "original-program") == 0);
    assert(operation.state == UMI_DEVELOPER_OPERATION_QUEUED && operation.attempt_count == 0U);
    assert(operation.argument_count == 2U && operation.arguments[0][0] == '\0');
    assert(strcmp(operation.arguments[1], "two words") == 0);
    strcpy(draft.program, "changed-again");
    assert(umi_project_launch_profile_registry_upsert(registry, &draft) == UMI_STATUS_OK);
    assert(umi_developer_pipeline_find(umi_developer_runtime_pipeline(runtime),
        plan.operation_id, &operation) == UMI_STATUS_OK);
    assert(strcmp(operation.program, "original-program") == 0);
    memcpy(&sentinel, &plan, sizeof(plan));
    assert(umi_project_launch_profile_registry_remove(registry, draft.id) == UMI_STATUS_OK);
    assert(umi_developer_launch_plan_submit_reviewed(runtime, &request, &expected,
        scratch, sizeof(scratch), &plan) == UMI_STATUS_NOT_FOUND);
    assert(memcmp(&plan, &sentinel, sizeof(plan)) == 0);
    umi_developer_runtime_destroy(runtime);
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    reviewed_launch_cases();
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

    /* Launch profiles preserve empty arguments on both sides of a file path. */
    strcpy(profile.id, "studio.empty");
    strcpy(profile.arguments, "\"\" \"source file.c\" ''");
    assert(umi_project_launch_profile_registry_upsert(
        umi_project_workspace_launch_profile(umi_developer_runtime_projects(runtime)),
        &profile) == UMI_STATUS_OK);
    request.launch_profile_id = "studio.empty";
    assert(umi_developer_launch_plan_submit(runtime, &request, &plan) == UMI_STATUS_OK);
    assert(umi_developer_pipeline_find(
        umi_developer_runtime_pipeline(runtime), plan.operation_id, &operation)
        == UMI_STATUS_OK);
    assert(operation.argument_count == 3U);
    assert(operation.arguments[0][0] == '\0');
    assert(strcmp(operation.arguments[1], "source file.c") == 0);
    assert(operation.arguments[2][0] == '\0');

    /* Malformed launch text cannot submit a partial debug operation. */
    {
        unsigned char before[sizeof(plan)];
        memcpy(before, &plan, sizeof(plan));
        strcpy(profile.id, "studio.malformed");
        strcpy(profile.arguments, "\"unfinished");
        assert(umi_project_launch_profile_registry_upsert(
            umi_project_workspace_launch_profile(umi_developer_runtime_projects(runtime)),
            &profile) == UMI_STATUS_OK);
        request.launch_profile_id = "studio.malformed";
        assert(umi_developer_launch_plan_submit(runtime, &request, &plan)
            == UMI_STATUS_PARSE_ERROR);
        assert(memcmp(before, &plan, sizeof(plan)) == 0);
        assert(umi_developer_pipeline_find(
            umi_developer_runtime_pipeline(runtime), "debug.studio.malformed", &operation)
            == UMI_STATUS_NOT_FOUND);
    }

    umi_developer_runtime_destroy(runtime);
    return 0;
}
