/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/runtime.h
 *
 * PURPOSE:
 *   Coordinate project, language, debugger, source-control, test and workbench
 *   services with the developer context, operation pipeline and event journal.
 *   The runtime can own those services for standalone applications or bind to
 *   service instances already owned by a product composition root.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Studio already owns several developer-facing service centres.  create_bound
 * lets Studio reuse those exact service objects instead of creating a second
 * disconnected set.  A smaller application can call create_owned and let the
 * developer runtime construct and destroy the complete service set itself.
 */
#ifndef UMICOM_DEVELOPER_RUNTIME_H
#define UMICOM_DEVELOPER_RUNTIME_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/developer/context.h"
#include "umicom/developer/cmake_plan.h"
#include "umicom/developer/executor.h"
#include "umicom/developer/journal.h"
#include "umicom/developer/pipeline.h"
#include "umicom/developer/universal_model.h"
#include "umicom/project/workspace.h"
#include "umicom/language/service.h"
#include "umicom/debug/service.h"
#include "umicom/source_control/service.h"
#include "umicom/test_platform/service.h"
#include "umicom/ui/workbench_services.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_RUNTIME_API_VERSION 1U

typedef struct UmiDeveloperRuntimeBindings {
    uint32_t struct_size;
    uint32_t api_version;
    UmiProjectWorkspace *projects;
    UmiLanguageService *language;
    UmiDebugService *debug;
    UmiSourceControlService *source_control;
    UmiTestPlatformService *tests;
    UmiUiWorkbenchServices *workbench;
} UmiDeveloperRuntimeBindings;

typedef struct UmiDeveloperRuntimeSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    UmiDeveloperContextSnapshot context;
    UmiDeveloperPipelineSnapshot pipeline;
    UmiDeveloperJournalSnapshot journal;
    UmiProjectWorkspaceSnapshot projects;
    UmiLanguageServiceSnapshot language;
    UmiDebugServiceSnapshot debug;
    UmiSourceControlServiceSnapshot source_control;
    UmiTestPlatformServiceSnapshot tests;
    UmiUiWorkbenchServicesSnapshot workbench;
    uint64_t revision;
    int owns_services;
} UmiDeveloperRuntimeSnapshot;

typedef struct UmiDeveloperRuntime UmiDeveloperRuntime;

void umi_developer_runtime_bindings_init(UmiDeveloperRuntimeBindings *bindings);

UmiStatus umi_developer_runtime_create_owned(
    UmiDeveloperRuntime **out_runtime);

UmiStatus umi_developer_runtime_create_bound(
    const UmiDeveloperRuntimeBindings *bindings,
    UmiDeveloperRuntime **out_runtime);

void umi_developer_runtime_destroy(UmiDeveloperRuntime *runtime);

UmiStatus umi_developer_runtime_set_context(
    UmiDeveloperRuntime *runtime,
    const UmiDeveloperContextSnapshot *context);

UmiStatus umi_developer_runtime_submit_operation(
    UmiDeveloperRuntime *runtime,
    const UmiDeveloperOperationSnapshot *operation);

UmiStatus umi_developer_runtime_add_dependency(
    UmiDeveloperRuntime *runtime,
    const char *operation_id,
    const char *depends_on_operation_id);

UmiStatus umi_developer_runtime_submit_cmake_plan(
    UmiDeveloperRuntime *runtime,
    const UmiDeveloperCMakePlanRequest *request,
    UmiDeveloperCMakePlanSnapshot *out_plan);

UmiStatus umi_developer_runtime_start_operation(
    UmiDeveloperRuntime *runtime,
    const char *operation_id);

UmiStatus umi_developer_runtime_start_next(
    UmiDeveloperRuntime *runtime,
    UmiDeveloperOperationSnapshot *out_operation);

UmiStatus umi_developer_runtime_set_progress(
    UmiDeveloperRuntime *runtime,
    const char *operation_id,
    uint32_t progress_basis_points,
    const char *summary);

UmiStatus umi_developer_runtime_complete_operation(
    UmiDeveloperRuntime *runtime,
    const char *operation_id,
    int exit_code,
    const char *summary);

UmiStatus umi_developer_runtime_fail_operation(
    UmiDeveloperRuntime *runtime,
    const char *operation_id,
    int exit_code,
    const char *summary);

UmiStatus umi_developer_runtime_cancel_operation(
    UmiDeveloperRuntime *runtime,
    const char *operation_id,
    const char *summary);

UmiStatus umi_developer_runtime_retry_operation(
    UmiDeveloperRuntime *runtime,
    const char *operation_id);

UmiStatus umi_developer_runtime_execute_next(
    UmiDeveloperRuntime *runtime,
    const UmiDeveloperExecutor *executor,
    UmiDeveloperOperationSnapshot *out_operation,
    UmiDeveloperExecutionResult *out_result);

UmiStatus umi_developer_runtime_snapshot(
    UmiDeveloperRuntime *runtime,
    UmiDeveloperRuntimeSnapshot *out_snapshot);

UmiDeveloperContext *umi_developer_runtime_context(UmiDeveloperRuntime *runtime);
UmiDeveloperPipeline *umi_developer_runtime_pipeline(UmiDeveloperRuntime *runtime);
UmiDeveloperJournal *umi_developer_runtime_journal(UmiDeveloperRuntime *runtime);
UmiProjectWorkspace *umi_developer_runtime_projects(UmiDeveloperRuntime *runtime);
UmiLanguageService *umi_developer_runtime_language(UmiDeveloperRuntime *runtime);
UmiDebugService *umi_developer_runtime_debug(UmiDeveloperRuntime *runtime);
UmiSourceControlService *umi_developer_runtime_source_control(UmiDeveloperRuntime *runtime);
UmiTestPlatformService *umi_developer_runtime_tests(UmiDeveloperRuntime *runtime);
UmiUiWorkbenchServices *umi_developer_runtime_workbench(UmiDeveloperRuntime *runtime);
UmiDeveloperUniversalModel *umi_developer_runtime_universal_model(
    UmiDeveloperRuntime *runtime);

#ifdef __cplusplus
}
#endif

#endif
