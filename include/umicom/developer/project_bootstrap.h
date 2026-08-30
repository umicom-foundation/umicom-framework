/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/project_bootstrap.h
 *
 * PURPOSE:
 *   Combine filesystem project import with the existing project-selection and
 *   developer-workflow services so a future Umicom application can open an
 *   existing source tree and optionally prepare a build/test/run workflow in a
 *   single reusable C23 operation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Bootstrap prepares metadata and developer operations only.  It does not run
 * an external command unless the caller later asks the developer executor to
 * execute the prepared operation pipeline.
 */
#ifndef UMICOM_DEVELOPER_PROJECT_BOOTSTRAP_H
#define UMICOM_DEVELOPER_PROJECT_BOOTSTRAP_H

#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/developer/project_workflow.h"
#include "umicom/project/workspace_import.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_PROJECT_BOOTSTRAP_API_VERSION 1U

typedef struct UmiDeveloperProjectBootstrapRequest {
    uint32_t struct_size;
    uint32_t api_version;
    UmiProjectWorkspaceImportRequest project;
    UmiDeveloperProjectWorkflowPreset preset;
    const char *workflow_id;
    uint32_t timeout_ms;
    uint32_t max_attempts;
    int prepare_workflow;
    int include_configure;
} UmiDeveloperProjectBootstrapRequest;

typedef struct UmiDeveloperProjectBootstrapSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    UmiProjectWorkspaceImportSnapshot project;
    UmiDeveloperProjectWorkflowSnapshot workflow;
    UmiDeveloperContextSnapshot context;
    int workflow_prepared;
} UmiDeveloperProjectBootstrapSnapshot;

UmiStatus umi_developer_project_bootstrap(
    UmiDeveloperRuntime *runtime,
    const UmiDeveloperProjectBootstrapRequest *request,
    UmiDeveloperProjectBootstrapSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif
#endif
