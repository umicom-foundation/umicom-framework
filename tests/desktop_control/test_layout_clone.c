/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_control/test_layout_clone.c
 * PURPOSE: Validate the Framework-owned layout clone contract.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/desktop/control/layout_clone.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDesktopLayoutCloneSnapshot value;
    UmiDesktopLayoutCloneSnapshot copy;
    umi_desktop_layout_clone_init(&value, "layout_clone.primary");
    assert(umi_desktop_layout_clone_validate(&value) == UMI_STATUS_OK);
    assert(umi_desktop_layout_clone_set_subject(&value, "desk.subject") == UMI_STATUS_OK);
    assert(umi_desktop_layout_clone_set_detail(&value, "Framework-owned control state") == UMI_STATUS_OK);
    assert(umi_desktop_layout_clone_set_state(&value, 2U) == UMI_STATUS_OK);
    assert(umi_desktop_layout_clone_set_priority(&value, 50U) == UMI_STATUS_OK);
    copy = value;
    assert(umi_desktop_layout_clone_same_identity(&value, &copy));
    assert(strcmp(value.subject_id, "desk.subject") == 0);
    return 0;
}
