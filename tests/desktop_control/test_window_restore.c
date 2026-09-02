/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_control/test_window_restore.c
 * PURPOSE: Validate the Framework-owned window restore contract.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/desktop/control/window_restore.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDesktopWindowRestoreSnapshot value;
    UmiDesktopWindowRestoreSnapshot copy;
    umi_desktop_window_restore_init(&value, "window_restore.primary");
    assert(umi_desktop_window_restore_validate(&value) == UMI_STATUS_OK);
    assert(umi_desktop_window_restore_set_subject(&value, "desk.subject") == UMI_STATUS_OK);
    assert(umi_desktop_window_restore_set_detail(&value, "Framework-owned control state") == UMI_STATUS_OK);
    assert(umi_desktop_window_restore_set_state(&value, 2U) == UMI_STATUS_OK);
    assert(umi_desktop_window_restore_set_priority(&value, 50U) == UMI_STATUS_OK);
    copy = value;
    assert(umi_desktop_window_restore_same_identity(&value, &copy));
    assert(strcmp(value.subject_id, "desk.subject") == 0);
    assert(umi_desktop_window_restore_should_restore(true, true, true));
    return 0;
}
