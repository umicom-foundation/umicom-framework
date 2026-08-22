/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_inbox_pressure.c
 *
 * PURPOSE:
 *   Verify bounded panel inboxes evict old deliveries and count pressure drops.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/workbench_context_host/host.h"
#include <stdio.h>

#include "test_support.h"

int main(void)
{
    UmiWorkbenchContextHostTestFixture fixture;
    UmiContextPayload payload;
    const UmiWorkbenchContextHostInbox *inbox;
    size_t index;
    assert(umi_wch_test_fixture_init(&fixture) == UMI_STATUS_OK);
    for (index = 0U;
         index < UMI_WORKBENCH_CONTEXT_HOST_MAX_INBOX_ITEMS + 12U;
         ++index) {
        char context_id[64U];
        int written = snprintf(context_id, sizeof(context_id), "selection-%zu", index);
        assert(written > 0 && (size_t)written < sizeof(context_id));
        assert(umi_wch_test_selection_payload(
            &payload, context_id, "test.source") == UMI_STATUS_OK);
        assert(umi_workbench_context_host_publish(
            fixture.host, "blue", "test.source", &payload,
            (uint64_t)(100U + index)) == UMI_STATUS_OK);
    }
    inbox = umi_workbench_context_host_inbox_registry_find_const(
        &fixture.host->inboxes, "test.target.endpoint");
    assert(inbox != NULL);
    assert(inbox->count == UMI_WORKBENCH_CONTEXT_HOST_MAX_INBOX_ITEMS);
    assert(inbox->dropped_count == 12U);
    umi_wch_test_fixture_destroy(&fixture);

    return 0;
}
