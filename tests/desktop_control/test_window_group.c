/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_control/test_window_group.c
 * PURPOSE: Validate the Framework-owned window group contract.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/desktop/control/window_group.h"

int main(void)
{
    UmiDesktopWindowGroupSnapshot value;
    UmiDesktopWindowGroupSnapshot copy;
    umi_desktop_window_group_init(&value, "window_group.primary");
    assert(umi_desktop_window_group_validate(&value) == UMI_STATUS_OK);
    assert(umi_desktop_window_group_set_subject(&value, "desk.subject") == UMI_STATUS_OK);
    assert(umi_desktop_window_group_set_detail(&value, "Framework-owned control state") == UMI_STATUS_OK);
    assert(umi_desktop_window_group_set_state(&value, 2U) == UMI_STATUS_OK);
    assert(umi_desktop_window_group_set_priority(&value, 50U) == UMI_STATUS_OK);
    copy = value;
    assert(umi_desktop_window_group_same_identity(&value, &copy));
    assert(strcmp(value.subject_id, "desk.subject") == 0);
    return 0;
}
