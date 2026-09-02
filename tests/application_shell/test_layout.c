/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_shell/test_layout.c
 *
 * PURPOSE:
 *   Verify reusable contribution placement and accepted drag/drop movement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/application_shell/layout.h"
#include "umicom/application_shell/drag_drop.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiApplicationShellLayout layout;
    UmiApplicationShellPlacement placement;
    UmiApplicationShellDropTransaction transaction;

    umi_application_shell_layout_init(
        &layout, "developer", "Developer Layout");

    assert(umi_application_shell_layout_place(
        &layout,
        "project.explorer",
        "primary",
        UMI_APPLICATION_SHELL_REGION_PRIMARY_SIDEBAR,
        0U,
        1) == UMI_STATUS_OK);

    assert(umi_application_shell_drop_prepare(
        &transaction,
        "drop.1",
        "project.explorer",
        "secondary",
        UMI_APPLICATION_SHELL_REGION_SECONDARY_SIDEBAR,
        UMI_APPLICATION_SHELL_DROP_MOVE,
        1U) == UMI_STATUS_OK);
    assert(umi_application_shell_drop_accept(
        &transaction) == UMI_STATUS_OK);

    assert(umi_application_shell_layout_move(
        &layout, &transaction) == UMI_STATUS_OK);
    assert(umi_application_shell_layout_find(
        &layout,
        "project.explorer",
        &placement) == UMI_STATUS_OK);
    assert(placement.region ==
           UMI_APPLICATION_SHELL_REGION_SECONDARY_SIDEBAR);
    assert(placement.order == 1U);

    return 0;
}
