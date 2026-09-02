/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web_workbench/test_request.c
 * PURPOSE: Verify editable API requests, validation and environment expansion.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/web/workbench/request.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiWebWorkbenchRequest request;
    UmiWebWorkbenchRequest resolved;
    UmiWebWorkbenchEnvironment environment;
    size_t unresolved = 0U;
    char message[128U];
    umi_web_workbench_request_init(&request, "orders", "Orders", UMI_HTTP_METHOD_POST,
        "{{baseUrl}}/orders");
    assert(umi_web_workbench_request_set_header(&request, "Accept", "application/json") == UMI_STATUS_OK);
    assert(umi_web_workbench_request_set_body(&request, UMI_WEB_WORKBENCH_BODY_JSON,
        "{\"book\":\"{{book}}\"}") == UMI_STATUS_OK);
    umi_web_workbench_environment_init(&environment, "dev", "Development");
    assert(umi_web_workbench_environment_set(&environment, "baseUrl",
        "https://api.example.test", true) == UMI_STATUS_OK);
    assert(umi_web_workbench_environment_set(&environment, "book", "LONDON", true) == UMI_STATUS_OK);
    assert(umi_web_workbench_request_resolve(&request, &environment, &resolved,
        &unresolved) == UMI_STATUS_OK);
    assert(unresolved == 0U);
    assert(strcmp(resolved.url, "https://api.example.test/orders") == 0);
    assert(strstr(resolved.body, "LONDON") != NULL);
    assert(umi_web_workbench_request_validate(&resolved, message, sizeof(message)) == UMI_STATUS_OK);
    assert(umi_web_workbench_request_remove_header(&resolved, "Accept") == UMI_STATUS_OK);
    return 0;
}
