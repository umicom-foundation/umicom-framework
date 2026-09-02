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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
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

/**
 * Represent the developer runtime bindings data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the developer runtime snapshot data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the developer runtime data shared with callers of this public contract.
 */
typedef struct UmiDeveloperRuntime UmiDeveloperRuntime;

/**
 * Initialise developer runtime bindings from caller-provided values so later operations
 * receive a known state.
 */
void umi_developer_runtime_bindings_init(UmiDeveloperRuntimeBindings *bindings);

/**
 * Provide the developer runtime create owned operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_runtime_create_owned(
    UmiDeveloperRuntime **out_runtime);

/**
 * Provide the developer runtime create bound operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_runtime_create_bound(
    const UmiDeveloperRuntimeBindings *bindings,
    UmiDeveloperRuntime **out_runtime);

/**
 * Release or reset state held by developer runtime so the same storage can be reused
 * safely.
 */
void umi_developer_runtime_destroy(UmiDeveloperRuntime *runtime);

/**
 * Provide the developer runtime set context operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_runtime_set_context(
    UmiDeveloperRuntime *runtime,
    const UmiDeveloperContextSnapshot *context);

/**
 * Provide the developer runtime submit operation operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_runtime_submit_operation(
    UmiDeveloperRuntime *runtime,
    const UmiDeveloperOperationSnapshot *operation);

/**
 * Provide the developer runtime add dependency operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_runtime_add_dependency(
    UmiDeveloperRuntime *runtime,
    const char *operation_id,
    const char *depends_on_operation_id);

/**
 * Provide the developer runtime submit cmake plan operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_runtime_submit_cmake_plan(
    UmiDeveloperRuntime *runtime,
    const UmiDeveloperCMakePlanRequest *request,
    UmiDeveloperCMakePlanSnapshot *out_plan);

/**
 * Provide the developer runtime start operation operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_runtime_start_operation(
    UmiDeveloperRuntime *runtime,
    const char *operation_id);

/**
 * Provide the developer runtime start next operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_runtime_start_next(
    UmiDeveloperRuntime *runtime,
    UmiDeveloperOperationSnapshot *out_operation);

/**
 * Provide the developer runtime set progress operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_runtime_set_progress(
    UmiDeveloperRuntime *runtime,
    const char *operation_id,
    uint32_t progress_basis_points,
    const char *summary);

/**
 * Provide the developer runtime complete operation operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_runtime_complete_operation(
    UmiDeveloperRuntime *runtime,
    const char *operation_id,
    int exit_code,
    const char *summary);

/**
 * Provide the developer runtime fail operation operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_runtime_fail_operation(
    UmiDeveloperRuntime *runtime,
    const char *operation_id,
    int exit_code,
    const char *summary);

/**
 * Provide the developer runtime cancel operation operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_runtime_cancel_operation(
    UmiDeveloperRuntime *runtime,
    const char *operation_id,
    const char *summary);

/**
 * Provide the developer runtime retry operation operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_runtime_retry_operation(
    UmiDeveloperRuntime *runtime,
    const char *operation_id);

/**
 * Provide the developer runtime execute next operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_runtime_execute_next(
    UmiDeveloperRuntime *runtime,
    const UmiDeveloperExecutor *executor,
    UmiDeveloperOperationSnapshot *out_operation,
    UmiDeveloperExecutionResult *out_result);

/**
 * Provide the developer runtime snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_runtime_snapshot(
    UmiDeveloperRuntime *runtime,
    UmiDeveloperRuntimeSnapshot *out_snapshot);

/**
 * Provide the developer runtime context operation used by this module and its client
 * applications.
 */
UmiDeveloperContext *umi_developer_runtime_context(UmiDeveloperRuntime *runtime);
/**
 * Provide the developer runtime pipeline operation used by this module and its client
 * applications.
 */
UmiDeveloperPipeline *umi_developer_runtime_pipeline(UmiDeveloperRuntime *runtime);
/**
 * Provide the developer runtime journal operation used by this module and its client
 * applications.
 */
UmiDeveloperJournal *umi_developer_runtime_journal(UmiDeveloperRuntime *runtime);
/**
 * Provide the developer runtime projects operation used by this module and its client
 * applications.
 */
UmiProjectWorkspace *umi_developer_runtime_projects(UmiDeveloperRuntime *runtime);
/**
 * Provide the developer runtime language operation used by this module and its client
 * applications.
 */
UmiLanguageService *umi_developer_runtime_language(UmiDeveloperRuntime *runtime);
/**
 * Provide the developer runtime debug operation used by this module and its client
 * applications.
 */
UmiDebugService *umi_developer_runtime_debug(UmiDeveloperRuntime *runtime);
/**
 * Provide the developer runtime source control operation used by this module and its
 * client applications.
 */
UmiSourceControlService *umi_developer_runtime_source_control(UmiDeveloperRuntime *runtime);
/**
 * Provide the developer runtime tests operation used by this module and its client
 * applications.
 */
UmiTestPlatformService *umi_developer_runtime_tests(UmiDeveloperRuntime *runtime);
/**
 * Provide the developer runtime workbench operation used by this module and its client
 * applications.
 */
UmiUiWorkbenchServices *umi_developer_runtime_workbench(UmiDeveloperRuntime *runtime);
/**
 * Provide the developer runtime universal model operation used by this module and its
 * client applications.
 */
UmiDeveloperUniversalModel *umi_developer_runtime_universal_model(
    UmiDeveloperRuntime *runtime);

#ifdef __cplusplus
}
#endif

#endif
