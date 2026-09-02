/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/workbench/transport.h
 *
 * PURPOSE:
 *   Define the provider-neutral client transport seam. Curl, libsoup, native
 *   sockets, test doubles and remote agents can execute the same request model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_WEB_WORKBENCH_TRANSPORT_H
#define UMICOM_WEB_WORKBENCH_TRANSPORT_H

#include "umicom/web/workbench/request.h"
#include "umicom/web/workbench/response.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef UmiStatus (*UmiWebWorkbenchTransportExecuteFn)(
    void *context,
    const UmiWebWorkbenchRequest *request,
    const volatile bool *cancel_requested,
    UmiWebWorkbenchResponse *out_response);

/**
 * Represent the web workbench transport data shared with callers of this public contract.
 */
typedef struct UmiWebWorkbenchTransport {
    char provider_id[UMI_WEB_WORKBENCH_ID_CAPACITY];
    void *context;
    UmiWebWorkbenchTransportExecuteFn execute;
    bool supports_tls;
    bool supports_streaming;
} UmiWebWorkbenchTransport;

/**
 * Initialise web workbench transport from caller-provided values so later operations
 * receive a known state.
 */
void umi_web_workbench_transport_init(
    UmiWebWorkbenchTransport *transport,
    const char *provider_id,
    UmiWebWorkbenchTransportExecuteFn execute,
    void *context);
/**
 * Check that web workbench transport satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_web_workbench_transport_validate(
    const UmiWebWorkbenchTransport *transport,
    const UmiWebWorkbenchRequest *request);
/**
 * Perform web workbench transport through the module contract so client applications do
 * not duplicate its policy.
 */
UmiStatus umi_web_workbench_transport_execute(
    const UmiWebWorkbenchTransport *transport,
    const UmiWebWorkbenchRequest *request,
    const volatile bool *cancel_requested,
    UmiWebWorkbenchResponse *out_response);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_WEB_WORKBENCH_TRANSPORT_H */
