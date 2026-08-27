/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/workbench/transport.h
 *
 * PURPOSE:
 *   Define the provider-neutral client transport seam. Curl, libsoup, native
 *   sockets, test doubles and remote agents can execute the same request model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiWebWorkbenchTransport {
    char provider_id[UMI_WEB_WORKBENCH_ID_CAPACITY];
    void *context;
    UmiWebWorkbenchTransportExecuteFn execute;
    bool supports_tls;
    bool supports_streaming;
} UmiWebWorkbenchTransport;

void umi_web_workbench_transport_init(
    UmiWebWorkbenchTransport *transport,
    const char *provider_id,
    UmiWebWorkbenchTransportExecuteFn execute,
    void *context);
UmiStatus umi_web_workbench_transport_validate(
    const UmiWebWorkbenchTransport *transport,
    const UmiWebWorkbenchRequest *request);
UmiStatus umi_web_workbench_transport_execute(
    const UmiWebWorkbenchTransport *transport,
    const UmiWebWorkbenchRequest *request,
    const volatile bool *cancel_requested,
    UmiWebWorkbenchResponse *out_response);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_WEB_WORKBENCH_TRANSPORT_H */
