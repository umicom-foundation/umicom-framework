/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/workbench/runtime.c
 *
 * PURPOSE:
 *   Implement the Framework-owned API workbench coordinator. It expands public
 *   variables, applies transient authentication, delegates transport execution
 *   and records only secret-free history evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

UmiStatus umi_web_workbench_runtime_create(
    UmiWebWorkbenchRuntime **out_runtime)
{
    UmiWebWorkbenchRuntime *runtime;
    if (out_runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_runtime = NULL;
    runtime = (UmiWebWorkbenchRuntime *)calloc(1U, sizeof(*runtime));
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

void umi_web_workbench_runtime_destroy(UmiWebWorkbenchRuntime *runtime)
{
    if (runtime == NULL) return;
    memset(runtime, 0, sizeof(*runtime));
    free(runtime);
}

UmiWebWorkbenchEnvironment *umi_web_workbench_runtime_environment(
    UmiWebWorkbenchRuntime *runtime)
{
    return runtime != NULL ? &runtime->environment : NULL;
}

UmiWebWorkbenchAuthCatalogue *umi_web_workbench_runtime_auth(
    UmiWebWorkbenchRuntime *runtime)
{
    return runtime != NULL ? &runtime->auth : NULL;
}

UmiWebWorkbenchCollection *umi_web_workbench_runtime_collection(
    UmiWebWorkbenchRuntime *runtime)
{
    return runtime != NULL ? &runtime->collection : NULL;
}

UmiWebWorkbenchHistory *umi_web_workbench_runtime_history(
    UmiWebWorkbenchRuntime *runtime)
{
    return runtime != NULL ? &runtime->history : NULL;
}

UmiWebWorkbenchOpenApiDocument *umi_web_workbench_runtime_openapi(
    UmiWebWorkbenchRuntime *runtime)
{
    return runtime != NULL ? &runtime->openapi : NULL;
}

UmiWebWorkbenchSoapCatalogue *umi_web_workbench_runtime_soap(
    UmiWebWorkbenchRuntime *runtime)
{
    return runtime != NULL ? &runtime->soap : NULL;
}

static UmiStatus fail_execution(
    UmiWebWorkbenchRuntime *runtime,
    uint64_t execution_id,
    UmiStatus status,
    const char *message,
    UmiWebWorkbenchResponse *out_response)
{
    umi_web_workbench_response_init(out_response);
    umi_web_workbench_response_complete(out_response, status, 0, NULL, false, message);
    if (execution_id != 0U) {
        (void)umi_web_workbench_history_complete(&runtime->history,
            execution_id, out_response);
    }
    runtime->last_status = status;
    runtime->phase = status == UMI_STATUS_CANCELLED
        ? UMI_WEB_WORKBENCH_CANCELLED : UMI_WEB_WORKBENCH_FAILED;
    if (status == UMI_STATUS_CANCELLED) runtime->cancelled_execution_count++;
    else runtime->failed_execution_count++;
    runtime->revision++;
    return status;
}

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
    if (runtime == NULL || transport == NULL || request == NULL || out_response == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (runtime->phase == UMI_WEB_WORKBENCH_RUNNING ||
        runtime->phase == UMI_WEB_WORKBENCH_PREPARING) return UMI_STATUS_BUSY;
    runtime->phase = UMI_WEB_WORKBENCH_PREPARING;
    runtime->cancel_requested = false;
    runtime->execution_count++;
    runtime->revision++;
    status = umi_web_workbench_request_resolve(request, &runtime->environment,
        &resolved, &unresolved);
    if (status != UMI_STATUS_OK) {
        return fail_execution(runtime, 0U, status,
            "Environment expansion failed.", out_response);
    }
    execution_id = umi_web_workbench_history_begin(&runtime->history, &resolved);
    if (execution_id == 0U) {
        return fail_execution(runtime, 0U, UMI_STATUS_INTERNAL_ERROR,
            "Execution history could not be started.", out_response);
    }
    if (unresolved > 0U) {
        return fail_execution(runtime, execution_id, UMI_STATUS_NOT_FOUND,
            "Request contains unresolved environment variables.", out_response);
    }
    prepared = resolved;
    if (prepared.auth_profile_id[0] != '\0') {
        profile = umi_web_workbench_auth_catalogue_find(
            &runtime->auth, prepared.auth_profile_id);
        if (profile == NULL) {
            return fail_execution(runtime, execution_id, UMI_STATUS_NOT_FOUND,
                "Authentication profile was not found.", out_response);
        }
        status = umi_web_workbench_auth_apply(profile, transient_secret, &prepared);
        if (status != UMI_STATUS_OK) {
            return fail_execution(runtime, execution_id, status,
                "Authentication could not be applied.", out_response);
        }
    }
    status = umi_web_workbench_transport_validate(transport, &prepared);
    if (status != UMI_STATUS_OK) {
        return fail_execution(runtime, execution_id, status,
            "Transport cannot execute this request.", out_response);
    }
    runtime->phase = UMI_WEB_WORKBENCH_RUNNING;
    runtime->revision++;
    status = umi_web_workbench_transport_execute(transport, &prepared,
        &runtime->cancel_requested, out_response);
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
    if (status == UMI_STATUS_CANCELLED) runtime->cancelled_execution_count++;
    else if (status != UMI_STATUS_OK) runtime->failed_execution_count++;
    runtime->revision++;
    memset(&prepared, 0, sizeof(prepared));
    return status;
}

UmiStatus umi_web_workbench_runtime_cancel(UmiWebWorkbenchRuntime *runtime)
{
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (runtime->phase != UMI_WEB_WORKBENCH_RUNNING &&
        runtime->phase != UMI_WEB_WORKBENCH_PREPARING) return UMI_STATUS_INVALID_STATE;
    runtime->cancel_requested = true;
    runtime->revision++;
    return UMI_STATUS_OK;
}

UmiStatus umi_web_workbench_runtime_snapshot(
    const UmiWebWorkbenchRuntime *runtime,
    UmiWebWorkbenchRuntimeSnapshot *out_snapshot)
{
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
