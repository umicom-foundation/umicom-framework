/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web_workbench/test_transport.c
 * PURPOSE: Verify provider-neutral execution and TLS capability validation.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/web/workbench/transport.h"

/*
 * Exercise execute stub and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiStatus execute_stub(
    void *context,
    const UmiWebWorkbenchRequest *request,
    const volatile bool *cancel_requested,
    UmiWebWorkbenchResponse *out_response)
{
    UmiWebWorkbenchTiming timing = {0U, 10U, 20U, 30U, 60U};
    int *calls = (int *)context;
    assert(request != NULL && cancel_requested != NULL);
    (*calls)++;
    assert(umi_web_workbench_response_set_body(out_response, "ok", 2U, 2U) == UMI_STATUS_OK);
    umi_web_workbench_response_complete(out_response, UMI_STATUS_OK, 200,
        &timing, true, NULL);
    return UMI_STATUS_OK;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiWebWorkbenchTransport transport;
    UmiWebWorkbenchRequest request;
    UmiWebWorkbenchResponse response;
    volatile bool cancel_requested = false;
    int calls = 0;
    umi_web_workbench_transport_init(&transport, "stub", execute_stub, &calls);
    umi_web_workbench_request_init(&request, "health", "Health",
        UMI_HTTP_METHOD_GET, "https://api.example.test/health");
    assert(umi_web_workbench_transport_execute(&transport, &request,
        &cancel_requested, &response) == UMI_STATUS_OK);
    assert(calls == 1 && response.http_status == 200);
    transport.supports_tls = false;
    assert(umi_web_workbench_transport_validate(&transport, &request) == UMI_STATUS_UNAVAILABLE);
    return 0;
}
