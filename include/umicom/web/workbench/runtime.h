/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/workbench/runtime.h
 *
 * PURPOSE:
 *   Coordinate environment expansion, transient authentication, transport
 *   execution, cancellation and bounded history for the API workbench.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_WEB_WORKBENCH_RUNTIME_H
#define UMICOM_WEB_WORKBENCH_RUNTIME_H

#include "umicom/web/workbench/auth_profile.h"
#include "umicom/web/workbench/cloud_object.h"
#include "umicom/web/workbench/cloud_queue.h"
#include "umicom/web/workbench/collection.h"
#include "umicom/web/workbench/history.h"
#include "umicom/web/workbench/openapi.h"
#include "umicom/web/workbench/server_diagnostics.h"
#include "umicom/web/workbench/soap.h"
#include "umicom/web/workbench/stream.h"
#include "umicom/web/workbench/transport.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWebWorkbenchRuntimeSnapshot {
    UmiWebWorkbenchPhase phase;
    UmiStatus last_status;
    uint64_t execution_count;
    uint64_t failed_execution_count;
    uint64_t cancelled_execution_count;
    size_t collection_request_count;
    size_t history_count;
    size_t openapi_operation_count;
    size_t soap_operation_count;
    bool cancel_requested;
    uint64_t revision;
} UmiWebWorkbenchRuntimeSnapshot;

typedef struct UmiWebWorkbenchRuntime UmiWebWorkbenchRuntime;

UmiStatus umi_web_workbench_runtime_create(
    UmiWebWorkbenchRuntime **out_runtime);
void umi_web_workbench_runtime_destroy(UmiWebWorkbenchRuntime *runtime);
UmiWebWorkbenchEnvironment *umi_web_workbench_runtime_environment(
    UmiWebWorkbenchRuntime *runtime);
UmiWebWorkbenchAuthCatalogue *umi_web_workbench_runtime_auth(
    UmiWebWorkbenchRuntime *runtime);
UmiWebWorkbenchCollection *umi_web_workbench_runtime_collection(
    UmiWebWorkbenchRuntime *runtime);
UmiWebWorkbenchHistory *umi_web_workbench_runtime_history(
    UmiWebWorkbenchRuntime *runtime);
UmiWebWorkbenchOpenApiDocument *umi_web_workbench_runtime_openapi(
    UmiWebWorkbenchRuntime *runtime);
UmiWebWorkbenchSoapCatalogue *umi_web_workbench_runtime_soap(
    UmiWebWorkbenchRuntime *runtime);
UmiStatus umi_web_workbench_runtime_execute(
    UmiWebWorkbenchRuntime *runtime,
    const UmiWebWorkbenchTransport *transport,
    const UmiWebWorkbenchRequest *request,
    const char *transient_secret,
    UmiWebWorkbenchResponse *out_response);
UmiStatus umi_web_workbench_runtime_cancel(UmiWebWorkbenchRuntime *runtime);
UmiStatus umi_web_workbench_runtime_snapshot(
    const UmiWebWorkbenchRuntime *runtime,
    UmiWebWorkbenchRuntimeSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_WEB_WORKBENCH_RUNTIME_H */
