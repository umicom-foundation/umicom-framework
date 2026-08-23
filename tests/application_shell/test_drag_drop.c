/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_shell/test_drag_drop.c
 *
 * PURPOSE:
 *   Verify portable move/copy/link drag/drop intent.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/application_shell/drag_drop.h"

int main(void)
{
    UmiApplicationShellDropTransaction transaction;

    assert(umi_application_shell_drop_prepare(
        &transaction,
        "drop.1",
        "umicom.shell.project-explorer.explorer",
        "secondary",
        UMI_APPLICATION_SHELL_REGION_SECONDARY_SIDEBAR,
        UMI_APPLICATION_SHELL_DROP_MOVE,
        2U) == UMI_STATUS_OK);

    assert(umi_application_shell_drop_accept(
        &transaction) == UMI_STATUS_OK);
    assert(transaction.accepted == 1);

    assert(umi_application_shell_drop_reject(
        &transaction, "Policy rejected move.") == UMI_STATUS_OK);
    assert(transaction.accepted == 0);

    return 0;
}
