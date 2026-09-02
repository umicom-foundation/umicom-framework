/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/workbench/runtime.h
 *
 * PURPOSE:
 *   Coordinate environment expansion, transient authentication, transport
 *   execution, cancellation and bounded history for the API workbench.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the web workbench runtime snapshot data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the web workbench runtime data shared with callers of this public contract.
 */
typedef struct UmiWebWorkbenchRuntime UmiWebWorkbenchRuntime;

/**
 * Initialise web workbench runtime from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_web_workbench_runtime_create(
    UmiWebWorkbenchRuntime **out_runtime);
/**
 * Release or reset state held by web workbench runtime so the same storage can be reused
 * safely.
 */
void umi_web_workbench_runtime_destroy(UmiWebWorkbenchRuntime *runtime);
/**
 * Provide the web workbench runtime environment operation used by this module and its
 * client applications.
 */
UmiWebWorkbenchEnvironment *umi_web_workbench_runtime_environment(
    UmiWebWorkbenchRuntime *runtime);
/**
 * Provide the web workbench runtime auth operation used by this module and its client
 * applications.
 */
UmiWebWorkbenchAuthCatalogue *umi_web_workbench_runtime_auth(
    UmiWebWorkbenchRuntime *runtime);
/**
 * Provide the web workbench runtime collection operation used by this module and its
 * client applications.
 */
UmiWebWorkbenchCollection *umi_web_workbench_runtime_collection(
    UmiWebWorkbenchRuntime *runtime);
/**
 * Provide the web workbench runtime history operation used by this module and its client
 * applications.
 */
UmiWebWorkbenchHistory *umi_web_workbench_runtime_history(
    UmiWebWorkbenchRuntime *runtime);
/**
 * Provide the web workbench runtime openapi operation used by this module and its client
 * applications.
 */
UmiWebWorkbenchOpenApiDocument *umi_web_workbench_runtime_openapi(
    UmiWebWorkbenchRuntime *runtime);
/**
 * Provide the web workbench runtime soap operation used by this module and its client
 * applications.
 */
UmiWebWorkbenchSoapCatalogue *umi_web_workbench_runtime_soap(
    UmiWebWorkbenchRuntime *runtime);
/**
 * Perform web workbench runtime through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_web_workbench_runtime_execute(
    UmiWebWorkbenchRuntime *runtime,
    const UmiWebWorkbenchTransport *transport,
    const UmiWebWorkbenchRequest *request,
    const char *transient_secret,
    UmiWebWorkbenchResponse *out_response);
/**
 * Provide the web workbench runtime cancel operation used by this module and its client
 * applications.
 */
UmiStatus umi_web_workbench_runtime_cancel(UmiWebWorkbenchRuntime *runtime);
/**
 * Provide the web workbench runtime snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_web_workbench_runtime_snapshot(
    const UmiWebWorkbenchRuntime *runtime,
    UmiWebWorkbenchRuntimeSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_WEB_WORKBENCH_RUNTIME_H */
