/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_developer_task_plan.c
 *
 * PURPOSE:
 *   Verify project tasks become executable developer operations.
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
#include "umicom/developer/task_plan.h"

#include <assert.h>
#include <string.h>

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDeveloperRuntime *runtime = NULL;
    UmiProjectTaskSnapshot task = {0};
    UmiDeveloperTaskPlanRequest request = {0};
    UmiDeveloperTaskPlanSnapshot plan;
    UmiDeveloperOperationSnapshot operation;

    assert(umi_developer_runtime_create_owned(&runtime) == UMI_STATUS_OK);
    task.struct_size = (uint32_t)sizeof(task);
    task.api_version = UMI_PROJECT_TASK_API_VERSION;
    strcpy(task.id, "build.core");
    strcpy(task.project_id, "studio");
    strcpy(task.label, "Build Core");
    strcpy(task.command, "cmake --build build --parallel 2");
    strcpy(task.working_directory, ".");
    strcpy(task.group, "build");
    task.enabled = 1;
    assert(umi_project_task_registry_upsert(
        umi_project_workspace_task(umi_developer_runtime_projects(runtime)),
        &task) == UMI_STATUS_OK);

    request.struct_size = (uint32_t)sizeof(request);
    request.api_version = UMI_DEVELOPER_TASK_PLAN_API_VERSION;
    request.task_id = "build.core";
    request.configuration_id = "debug";
    request.target_id = "core";
    request.max_attempts = 2U;
    assert(umi_developer_task_plan_submit(runtime, &request, &plan) == UMI_STATUS_OK);
    assert(strcmp(plan.operation_id, "task.build.core") == 0);
    assert(umi_developer_pipeline_find(
        umi_developer_runtime_pipeline(runtime), plan.operation_id, &operation)
        == UMI_STATUS_OK);
    assert(strcmp(operation.program, "cmake") == 0);
    assert(operation.argument_count == 4U);
    assert(operation.max_attempts == 2U);
    assert(strcmp(operation.configuration_id, "debug") == 0);

    /* Empty positional arguments must survive task-to-operation translation. */
    strcpy(task.id, "build.empty");
    strcpy(task.command, "clang \"\" \"source file.c\"");
    assert(umi_project_task_registry_upsert(
        umi_project_workspace_task(umi_developer_runtime_projects(runtime)),
        &task) == UMI_STATUS_OK);
    request.task_id = "build.empty";
    assert(umi_developer_task_plan_submit(runtime, &request, &plan) == UMI_STATUS_OK);
    assert(umi_developer_pipeline_find(
        umi_developer_runtime_pipeline(runtime), plan.operation_id, &operation)
        == UMI_STATUS_OK);
    assert(operation.argument_count == 2U);
    assert(operation.arguments[0][0] == '\0');
    assert(strcmp(operation.arguments[1], "source file.c") == 0);

    /* A rejected edit must neither publish an operation nor replace the plan. */
    {
        unsigned char before[sizeof(plan)];
        memcpy(before, &plan, sizeof(plan));
        strcpy(task.id, "build.malformed");
        strcpy(task.command, "clang \"unfinished");
        assert(umi_project_task_registry_upsert(
            umi_project_workspace_task(umi_developer_runtime_projects(runtime)),
            &task) == UMI_STATUS_OK);
        request.task_id = "build.malformed";
        assert(umi_developer_task_plan_submit(runtime, &request, &plan)
            == UMI_STATUS_PARSE_ERROR);
        assert(memcmp(before, &plan, sizeof(plan)) == 0);
        assert(umi_developer_pipeline_find(
            umi_developer_runtime_pipeline(runtime), "task.build.malformed", &operation)
            == UMI_STATUS_NOT_FOUND);
    }

    umi_developer_runtime_destroy(runtime);
    return 0;
}
