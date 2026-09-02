/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web_workbench/test_response.c
 * PURPOSE: Verify response timing, headers and bounded body truncation evidence.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/web/workbench/response.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiWebWorkbenchResponse response;
    UmiWebWorkbenchTiming timing = {10U, 20U, 30U, 40U, 100U};
    char large[UMI_WEB_WORKBENCH_PAYLOAD_CAPACITY + 32U];
    memset(large, 'x', sizeof(large));
    umi_web_workbench_response_init(&response);
    assert(umi_web_workbench_response_set_header(&response,
        "Content-Type", "application/json") == UMI_STATUS_OK);
    assert(umi_web_workbench_response_set_body(&response, large, sizeof(large),
        sizeof(large)) == UMI_STATUS_OK);
    assert(response.body_truncated);
    assert(response.body_length == UMI_WEB_WORKBENCH_PAYLOAD_CAPACITY - 1U);
    umi_web_workbench_response_complete(&response, UMI_STATUS_OK, 200,
        &timing, true, NULL);
    assert(response.phase == UMI_WEB_WORKBENCH_COMPLETED);
    assert(response.timing.total_us == 100U);
    return 0;
}
