/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_diagnostic_snapshot.c
 *
 * PURPOSE:
 *   Implement the test diagnostic snapshot behavior for
 *   Umicom Framework.
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
#include "umicom/diagnostics/diagnostic.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDiagnosticSnapshot item;
    char message[128];
    assert(umi_diagnostic_snapshot_init(&item, "d1", UMI_DIAGNOSTIC_ERROR,
        UMI_DIAGNOSTIC_KIND_COMPILER, "cc", "bad token") == UMI_STATUS_OK);
    assert(item.api_version == UMI_DIAGNOSTIC_API_VERSION);
    assert(strcmp(umi_diagnostic_kind_text(item.kind), "COMPILER") == 0);
    assert(umi_diagnostic_snapshot_validate(&item, message, sizeof(message)) == UMI_STATUS_OK);
    item.column = 2U;
    assert(umi_diagnostic_snapshot_validate(&item, NULL, 0U) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
