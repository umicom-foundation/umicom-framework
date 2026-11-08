/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/lifecycle_execution.h
 *
 * PURPOSE:
 *   Execute canonical lifecycle plans through synchronous host adapters with admission before side effects.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEVELOPER_LIFECYCLE_EXECUTION_H
#define UMICOM_DEVELOPER_LIFECYCLE_EXECUTION_H
#include "umicom/developer/lifecycle_plan.h"
#ifdef __cplusplus
extern "C" {
#endif

/* Several descriptive stages share one real backend invocation. In particular,
 * a repository publish is indivisible here: its nonzero result cannot prove
 * which of stage, commit and push completed. Consult that backend's evidence. */
typedef enum UmiDeveloperLifecycleAction {
    UMI_DEVELOPER_ACTION_NONE = 0,
    UMI_DEVELOPER_ACTION_ENVIRONMENT = 1,
    UMI_DEVELOPER_ACTION_SOURCE_SCAN = 2,
    UMI_DEVELOPER_ACTION_DEPENDENCY_AUDIT = 3,
    UMI_DEVELOPER_ACTION_CONFIGURE = 4,
    UMI_DEVELOPER_ACTION_BUILD = 5,
    UMI_DEVELOPER_ACTION_TEST = 6,
    UMI_DEVELOPER_ACTION_REPOSITORY_VERIFY = 7,
    UMI_DEVELOPER_ACTION_REPOSITORY_PUBLISH = 8,
    UMI_DEVELOPER_ACTION_REPOSITORY_UPDATE = 9,
    UMI_DEVELOPER_ACTION_COUNT = 10
} UmiDeveloperLifecycleAction;

typedef struct UmiDeveloperLifecycleExecutionRequest {
    UmiDeveloperLifecycleGoal goal;
    const char *source;
    const char *preset;
    const char *jobs;
    const char *message;
    const char *remote;
    const char *branch;
    int strict;
    int preview;
    int no_submodules;
} UmiDeveloperLifecycleExecutionRequest;

/* All strings are borrowed and must stay immutable until execution returns.
 * Callbacks are synchronous, return zero on success, and retain nothing.
 * Hosts supply existing qualified services, not shell command strings. */
typedef int (*UmiDeveloperLifecycleActionExecute)(void *user_data,
    const UmiDeveloperLifecycleExecutionRequest *request,
    UmiDeveloperLifecycleAction action);
typedef struct UmiDeveloperLifecycleExecutionHost {
    void *user_data;
    UmiDeveloperLifecycleActionExecute execute[UMI_DEVELOPER_ACTION_COUNT];
} UmiDeveloperLifecycleExecutionHost;

typedef struct UmiDeveloperLifecycleActionResult {
    UmiDeveloperLifecycleAction action;
    size_t first_stage;
    size_t stage_count;
    int attempted;
    int completed;
    int backend_result;
} UmiDeveloperLifecycleActionResult;
typedef struct UmiDeveloperLifecycleExecutionReport {
    UmiDeveloperLifecyclePlan plan;
    UmiDeveloperLifecycleActionResult actions[UMI_DEVELOPER_ACTION_COUNT - 1];
    size_t action_count;
    size_t attempted_count;
    size_t completed_count;
    UmiDeveloperLifecycleAction failed_action;
    int backend_result;
    UmiStatus status;
    int preview;
} UmiDeveloperLifecycleExecutionReport;

void umi_developer_lifecycle_execution_request_init(
    UmiDeveloperLifecycleExecutionRequest *request, UmiDeveloperLifecycleGoal goal);
/* Preview needs no host and invokes no callbacks. Execution checks every needed
 * callback before the first call, then stops at the first nonzero result.
 * Admission failure leaves zero attempted/completed counts. No rollback or
 * automatic retry is implied; prior successful actions may have changed state. */
UmiStatus umi_developer_lifecycle_execute(
    const UmiDeveloperLifecycleExecutionRequest *request,
    const UmiDeveloperLifecycleExecutionHost *host,
    UmiDeveloperLifecycleExecutionReport *out_report);
const char *umi_developer_lifecycle_action_text(UmiDeveloperLifecycleAction action);
#ifdef __cplusplus
}
#endif
#endif
