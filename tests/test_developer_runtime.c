/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_developer_runtime.c
 *
 * PURPOSE:
 *   Verify bound-service identity, executor-driven lifecycle, event journalling
 *   and automatic workbench progress/output/problem publication.
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

#include "umicom/developer/runtime.h"

/*
 * Exercise fake success and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiStatus fake_success(
    void *user_data,
    const UmiDeveloperOperationSnapshot *operation,
    UmiDeveloperExecutionResult *out_result)
{
    int *call_count = (int *)user_data;

    assert(operation != NULL);
    assert(out_result != NULL);
    *call_count += 1;

    memset(out_result, 0, sizeof(*out_result));
    out_result->struct_size = (uint32_t)sizeof(*out_result);
    out_result->api_version = UMI_DEVELOPER_EXECUTION_API_VERSION;
    out_result->launched = 1;
    out_result->exit_code = 0;
    strcpy(out_result->output, "fake execution succeeded");
    return UMI_STATUS_OK;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiProjectWorkspace *projects = NULL;
    UmiLanguageService *language = NULL;
    UmiDebugService *debug = NULL;
    UmiSourceControlService *source_control = NULL;
    UmiTestPlatformService *tests = NULL;
    UmiUiWorkbenchServices *workbench = NULL;
    UmiDeveloperRuntimeBindings bindings;
    UmiDeveloperRuntime *runtime = NULL;
    UmiDeveloperOperationSnapshot operation;
    UmiDeveloperOperationSnapshot final_operation;
    UmiDeveloperExecutionResult execution;
    UmiDeveloperExecutor executor;
    UmiDeveloperRuntimeSnapshot snapshot;
    UmiUiProgressSnapshot progress;
    UmiUiOutputChannelSnapshot output;
    int call_count = 0;

    assert(umi_project_workspace_create(&projects) == UMI_STATUS_OK);
    assert(umi_language_service_create(&language) == UMI_STATUS_OK);
    assert(umi_debug_service_create(&debug) == UMI_STATUS_OK);
    assert(umi_source_control_service_create(&source_control) == UMI_STATUS_OK);
    assert(umi_test_platform_service_create(&tests) == UMI_STATUS_OK);
    assert(umi_ui_workbench_services_create(&workbench) == UMI_STATUS_OK);

    umi_developer_runtime_bindings_init(&bindings);
    bindings.projects = projects;
    bindings.language = language;
    bindings.debug = debug;
    bindings.source_control = source_control;
    bindings.tests = tests;
    bindings.workbench = workbench;

    assert(umi_developer_runtime_create_bound(&bindings, &runtime) ==
           UMI_STATUS_OK);
    assert(umi_developer_runtime_projects(runtime) == projects);
    assert(umi_developer_runtime_language(runtime) == language);
    assert(umi_developer_runtime_debug(runtime) == debug);
    assert(umi_developer_runtime_source_control(runtime) == source_control);
    assert(umi_developer_runtime_tests(runtime) == tests);
    assert(umi_developer_runtime_workbench(runtime) == workbench);
    assert(umi_developer_runtime_universal_model(runtime) != NULL);

    assert(umi_developer_operation_init(
        &operation,
        "build.main",
        UMI_DEVELOPER_OPERATION_BUILD,
        "Build main target") == UMI_STATUS_OK);
    operation.max_attempts = 2U;
    assert(umi_developer_runtime_submit_operation(runtime, &operation) ==
           UMI_STATUS_OK);

    umi_developer_executor_init(&executor, fake_success, &call_count);
    assert(umi_developer_runtime_execute_next(
        runtime, &executor, &final_operation, &execution) == UMI_STATUS_OK);
    assert(call_count == 1);
    assert(final_operation.state == UMI_DEVELOPER_OPERATION_SUCCEEDED);
    assert(execution.exit_code == 0);

    assert(umi_developer_runtime_snapshot(runtime, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.owns_services == 0);
    assert(snapshot.pipeline.succeeded_count == 1U);
    assert(snapshot.journal.entry_count >= 3U);

    assert(umi_ui_progress_registry_find(
        umi_ui_workbench_services_progress(workbench),
        "build.main",
        &progress) == UMI_STATUS_OK);
    assert(progress.fraction == 1.0);

    assert(umi_ui_output_channel_registry_find(
        umi_ui_workbench_services_output_channel(workbench),
        "build.main",
        &output) == UMI_STATUS_OK);
    assert(strstr(output.text, "succeeded") != NULL);

    /*
     * Bound runtime destruction must not destroy product-owned services.
     * Snapshotting them afterwards proves ownership stayed with the caller.
     */
    umi_developer_runtime_destroy(runtime);
    assert(umi_project_workspace_snapshot(projects, &snapshot.projects) ==
           UMI_STATUS_OK);

    umi_ui_workbench_services_destroy(workbench);
    umi_test_platform_service_destroy(tests);
    umi_source_control_service_destroy(source_control);
    umi_debug_service_destroy(debug);
    umi_language_service_destroy(language);
    umi_project_workspace_destroy(projects);
    return 0;
}
