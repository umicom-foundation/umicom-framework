/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_diagnostics.c
 *
 * PURPOSE:
 *   Verify structured diagnostics retain severity and messages.
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
#include "umicom/workbench_context_host/diagnostics.h"

#include "test_support.h"

int main(void)
{
    UmiWorkbenchContextHostDiagnostics diagnostics;
    UmiWorkbenchContextHostDiagnostic item;
    memset(&item, 0, sizeof(item));
    umi_workbench_context_host_diagnostics_init(&diagnostics);
    assert(umi_workbench_context_host_copy_text(
        item.diagnostic_id, sizeof(item.diagnostic_id), "diagnostic") == UMI_STATUS_OK);
    assert(umi_workbench_context_host_copy_text(
        item.message, sizeof(item.message), "message") == UMI_STATUS_OK);
    item.severity = UMI_WORKBENCH_CONTEXT_HOST_DIAGNOSTIC_WARNING;
    assert(umi_workbench_context_host_diagnostics_add(
        &diagnostics, &item) == UMI_STATUS_OK);
    assert(diagnostics.count == 1U);
    assert(diagnostics.items[0].severity ==
        UMI_WORKBENCH_CONTEXT_HOST_DIAGNOSTIC_WARNING);

    return 0;
}
