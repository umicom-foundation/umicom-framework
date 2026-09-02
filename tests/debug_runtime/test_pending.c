/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_runtime/test_pending.c
 *
 * PURPOSE:
 *   Verify DAP request sequence correlation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/debug_runtime/pending.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDebugRuntimePendingTable table;
    UmiDebugRuntimePendingRequest request;

    umi_debug_runtime_pending_init(&table);
    assert(umi_debug_runtime_pending_add(
        &table, 5U, "threads", "session") == UMI_STATUS_OK);
    assert(table.active_count == 1U);
    assert(umi_debug_runtime_pending_take(
        &table, 5U, &request) == UMI_STATUS_OK);
    assert(strcmp(request.command, "threads") == 0);
    assert(table.active_count == 0U);
    return 0;
}
