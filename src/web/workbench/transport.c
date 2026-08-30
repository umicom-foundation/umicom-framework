/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/workbench/transport.c
 *
 * PURPOSE:
 *   Implement validation and invocation for provider-neutral client transports.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/web/workbench/transport.h"

#include <string.h>

void umi_web_workbench_transport_init(
    UmiWebWorkbenchTransport *transport,
    const char *provider_id,
    UmiWebWorkbenchTransportExecuteFn execute,
    void *context)
{
    if (transport == NULL) return;
    memset(transport, 0, sizeof(*transport));
    (void)umi_web_workbench_copy_text(transport->provider_id,
        sizeof(transport->provider_id), provider_id != NULL ? provider_id : "transport");
    transport->execute = execute;
    transport->context = context;
    transport->supports_tls = true;
}

UmiStatus umi_web_workbench_transport_validate(
    const UmiWebWorkbenchTransport *transport,
    const UmiWebWorkbenchRequest *request)
{
    UmiWebUrl parsed;
    UmiStatus status;
    if (transport == NULL || transport->execute == NULL ||
        transport->provider_id[0] == '\0' || request == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_web_workbench_request_validate(request, NULL, 0U);
    if (status != UMI_STATUS_OK) return status;
    status = umi_web_url_parse(request->url, &parsed);
    if (status != UMI_STATUS_OK) return status;
    if (strcmp(parsed.scheme, "https") == 0 && !transport->supports_tls) {
        return UMI_STATUS_UNAVAILABLE;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_web_workbench_transport_execute(
    const UmiWebWorkbenchTransport *transport,
    const UmiWebWorkbenchRequest *request,
    const volatile bool *cancel_requested,
    UmiWebWorkbenchResponse *out_response)
{
    UmiStatus status;
    if (out_response == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_web_workbench_transport_validate(transport, request);
    if (status != UMI_STATUS_OK) return status;
    if (cancel_requested != NULL && *cancel_requested) {
        umi_web_workbench_response_init(out_response);
        umi_web_workbench_response_complete(out_response, UMI_STATUS_CANCELLED,
            0, NULL, false, "Request cancelled before transport execution.");
        return UMI_STATUS_CANCELLED;
    }
    umi_web_workbench_response_init(out_response);
    out_response->phase = UMI_WEB_WORKBENCH_RUNNING;
    return transport->execute(transport->context, request,
        cancel_requested, out_response);
}
