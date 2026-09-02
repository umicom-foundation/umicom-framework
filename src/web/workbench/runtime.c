/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/workbench/runtime.c
 *
 * PURPOSE:
 *   Implement the Framework-owned API workbench coordinator. It expands public
 *   variables, applies transient authentication, delegates transport execution
 *   and records only secret-free history evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/web/workbench/runtime.h"

#include <stdlib.h>
#include <string.h>

struct UmiWebWorkbenchRuntime {
    UmiWebWorkbenchEnvironment environment;
    UmiWebWorkbenchAuthCatalogue auth;
    UmiWebWorkbenchCollection collection;
    UmiWebWorkbenchHistory history;
    UmiWebWorkbenchOpenApiDocument openapi;
    UmiWebWorkbenchSoapCatalogue soap;
    UmiWebWorkbenchPhase phase;
    UmiStatus last_status;
    uint64_t execution_count;
    uint64_t failed_execution_count;
    uint64_t cancelled_execution_count;
    volatile bool cancel_requested;
    uint64_t revision;
};

/*
 * Initialise web workbench runtime from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_web_workbench_runtime_create(
    UmiWebWorkbenchRuntime **out_runtime)
{
    UmiWebWorkbenchRuntime *runtime;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_runtime = NULL;
    runtime = (UmiWebWorkbenchRuntime *)calloc(1U, sizeof(*runtime));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    umi_web_workbench_environment_init(&runtime->environment, "default", "Default");
    umi_web_workbench_auth_catalogue_init(&runtime->auth);
    umi_web_workbench_collection_init(&runtime->collection, "workspace", "Workspace APIs");
    umi_web_workbench_history_init(&runtime->history);
    umi_web_workbench_openapi_init(&runtime->openapi, "workspace-openapi");
    umi_web_workbench_soap_catalogue_init(&runtime->soap);
    runtime->phase = UMI_WEB_WORKBENCH_IDLE;
    runtime->last_status = UMI_STATUS_OK;
    runtime->revision = 1U;
    *out_runtime = runtime;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by web workbench runtime so the same storage can be reused
 * safely.
 */
void umi_web_workbench_runtime_destroy(UmiWebWorkbenchRuntime *runtime)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return;
    memset(runtime, 0, sizeof(*runtime));
    free(runtime);
}

/*
 * Provide the web workbench runtime environment operation used by this module and its
 * client applications.
 */
UmiWebWorkbenchEnvironment *umi_web_workbench_runtime_environment(
    UmiWebWorkbenchRuntime *runtime)
{
    return runtime != NULL ? &runtime->environment : NULL;
}

/*
 * Provide the web workbench runtime auth operation used by this module and its client
 * applications.
 */
UmiWebWorkbenchAuthCatalogue *umi_web_workbench_runtime_auth(
    UmiWebWorkbenchRuntime *runtime)
{
    return runtime != NULL ? &runtime->auth : NULL;
}

/*
 * Provide the web workbench runtime collection operation used by this module and its
 * client applications.
 */
UmiWebWorkbenchCollection *umi_web_workbench_runtime_collection(
    UmiWebWorkbenchRuntime *runtime)
{
    return runtime != NULL ? &runtime->collection : NULL;
}

/*
 * Provide the web workbench runtime history operation used by this module and its client
 * applications.
 */
UmiWebWorkbenchHistory *umi_web_workbench_runtime_history(
    UmiWebWorkbenchRuntime *runtime)
{
    return runtime != NULL ? &runtime->history : NULL;
}

/*
 * Provide the web workbench runtime openapi operation used by this module and its client
 * applications.
 */
UmiWebWorkbenchOpenApiDocument *umi_web_workbench_runtime_openapi(
    UmiWebWorkbenchRuntime *runtime)
{
    return runtime != NULL ? &runtime->openapi : NULL;
}

/*
 * Provide the web workbench runtime soap operation used by this module and its client
 * applications.
 */
UmiWebWorkbenchSoapCatalogue *umi_web_workbench_runtime_soap(
    UmiWebWorkbenchRuntime *runtime)
{
    return runtime != NULL ? &runtime->soap : NULL;
}

/* Provide the fail execution operation used by this module and its client applications. */
static UmiStatus fail_execution(
    UmiWebWorkbenchRuntime *runtime,
    uint64_t execution_id,
    UmiStatus status,
    const char *message,
    UmiWebWorkbenchResponse *out_response)
{
    umi_web_workbench_response_init(out_response);
    umi_web_workbench_response_complete(out_response, status, 0, NULL, false, message);
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (execution_id != 0U) {
        (void)umi_web_workbench_history_complete(&runtime->history,
            execution_id, out_response);
    }
    runtime->last_status = status;
    runtime->phase = status == UMI_STATUS_CANCELLED
        ? UMI_WEB_WORKBENCH_CANCELLED : UMI_WEB_WORKBENCH_FAILED;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_CANCELLED) runtime->cancelled_execution_count++;
    /* Use this fallback path when the earlier condition does not apply. */
    else runtime->failed_execution_count++;
    runtime->revision++;
    return status;
}

/*
 * Perform web workbench runtime through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_web_workbench_runtime_execute(
    UmiWebWorkbenchRuntime *runtime,
    const UmiWebWorkbenchTransport *transport,
    const UmiWebWorkbenchRequest *request,
    const char *transient_secret,
    UmiWebWorkbenchResponse *out_response)
{
    UmiWebWorkbenchRequest resolved;
    UmiWebWorkbenchRequest prepared;
    const UmiWebWorkbenchAuthProfile *profile = NULL;
    size_t unresolved = 0U;
    uint64_t execution_id;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || transport == NULL || request == NULL || out_response == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (runtime->phase == UMI_WEB_WORKBENCH_RUNNING ||
        runtime->phase == UMI_WEB_WORKBENCH_PREPARING) return UMI_STATUS_BUSY;
    runtime->phase = UMI_WEB_WORKBENCH_PREPARING;
    runtime->cancel_requested = false;
    runtime->execution_count++;
    runtime->revision++;
    status = umi_web_workbench_request_resolve(request, &runtime->environment,
        &resolved, &unresolved);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return fail_execution(runtime, 0U, status,
            "Environment expansion failed.", out_response);
    }
    execution_id = umi_web_workbench_history_begin(&runtime->history, &resolved);
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (execution_id == 0U) {
        return fail_execution(runtime, 0U, UMI_STATUS_INTERNAL_ERROR,
            "Execution history could not be started.", out_response);
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (unresolved > 0U) {
        return fail_execution(runtime, execution_id, UMI_STATUS_NOT_FOUND,
            "Request contains unresolved environment variables.", out_response);
    }
    prepared = resolved;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (prepared.auth_profile_id[0] != '\0') {
        profile = umi_web_workbench_auth_catalogue_find(
            &runtime->auth, prepared.auth_profile_id);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (profile == NULL) {
            return fail_execution(runtime, execution_id, UMI_STATUS_NOT_FOUND,
                "Authentication profile was not found.", out_response);
        }
        status = umi_web_workbench_auth_apply(profile, transient_secret, &prepared);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            return fail_execution(runtime, execution_id, status,
                "Authentication could not be applied.", out_response);
        }
    }
    status = umi_web_workbench_transport_validate(transport, &prepared);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return fail_execution(runtime, execution_id, status,
            "Transport cannot execute this request.", out_response);
    }
    runtime->phase = UMI_WEB_WORKBENCH_RUNNING;
    runtime->revision++;
    status = umi_web_workbench_transport_execute(transport, &prepared,
        &runtime->cancel_requested, out_response);
    /* Apply this branch only when its contract condition is satisfied. */
    if (out_response->phase == UMI_WEB_WORKBENCH_RUNNING ||
        out_response->phase == UMI_WEB_WORKBENCH_IDLE) {
        umi_web_workbench_response_complete(out_response, status,
            out_response->http_status, &out_response->timing,
            out_response->tls_verified,
            status == UMI_STATUS_OK ? "" : "Transport execution failed.");
    }
    (void)umi_web_workbench_history_complete(&runtime->history,
        execution_id, out_response);
    runtime->last_status = status;
    runtime->phase = out_response->phase;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_CANCELLED) runtime->cancelled_execution_count++;
    else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) runtime->failed_execution_count++;
    runtime->revision++;
    memset(&prepared, 0, sizeof(prepared));
    return status;
}

/*
 * Provide the web workbench runtime cancel operation used by this module and its client
 * applications.
 */
UmiStatus umi_web_workbench_runtime_cancel(UmiWebWorkbenchRuntime *runtime)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (runtime->phase != UMI_WEB_WORKBENCH_RUNNING &&
        runtime->phase != UMI_WEB_WORKBENCH_PREPARING) return UMI_STATUS_INVALID_STATE;
    runtime->cancel_requested = true;
    runtime->revision++;
    return UMI_STATUS_OK;
}

/*
 * Provide the web workbench runtime snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_web_workbench_runtime_snapshot(
    const UmiWebWorkbenchRuntime *runtime,
    UmiWebWorkbenchRuntimeSnapshot *out_snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->phase = runtime->phase;
    out_snapshot->last_status = runtime->last_status;
    out_snapshot->execution_count = runtime->execution_count;
    out_snapshot->failed_execution_count = runtime->failed_execution_count;
    out_snapshot->cancelled_execution_count = runtime->cancelled_execution_count;
    out_snapshot->collection_request_count = runtime->collection.request_count;
    out_snapshot->history_count = runtime->history.entry_count;
    out_snapshot->openapi_operation_count = runtime->openapi.operation_count;
    out_snapshot->soap_operation_count = runtime->soap.operation_count;
    out_snapshot->cancel_requested = runtime->cancel_requested;
    out_snapshot->revision = runtime->revision;
    return UMI_STATUS_OK;
}
