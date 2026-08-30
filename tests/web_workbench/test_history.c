/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web_workbench/test_history.c
 * PURPOSE: Verify bounded newest-first execution history and filtering.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/web/workbench/history.h"

int main(void)
{
    UmiWebWorkbenchHistory history;
    UmiWebWorkbenchRequest request;
    UmiWebWorkbenchResponse response;
    UmiWebWorkbenchHistoryEntry matches[4U];
    UmiWebWorkbenchTiming timing = {0U, 0U, 0U, 0U, 123U};
    uint64_t id;
    umi_web_workbench_history_init(&history);
    umi_web_workbench_request_init(&request, "positions", "Positions",
        UMI_HTTP_METHOD_GET, "https://api.example.test/positions");
    id = umi_web_workbench_history_begin(&history, &request);
    assert(id != 0U);
    umi_web_workbench_response_init(&response);
    response.received_bytes = 512U;
    umi_web_workbench_response_complete(&response, UMI_STATUS_OK, 200,
        &timing, true, NULL);
    assert(umi_web_workbench_history_complete(&history, id, &response) == UMI_STATUS_OK);
    assert(umi_web_workbench_history_query(&history, "position", 200,
        matches, 4U) == 1U);
    assert(matches[0].duration_us == 123U);
    umi_web_workbench_history_clear(&history);
    assert(history.entry_count == 0U);
    return 0;
}
