/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_control/test_panel_visibility.c
 * PURPOSE: Validate the Framework-owned panel visibility contract.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/desktop/control/panel_visibility.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDesktopPanelVisibilitySnapshot value;
    UmiDesktopPanelVisibilitySnapshot copy;
    umi_desktop_panel_visibility_init(&value, "panel_visibility.primary");
    assert(umi_desktop_panel_visibility_validate(&value) == UMI_STATUS_OK);
    assert(umi_desktop_panel_visibility_set_subject(&value, "desk.subject") == UMI_STATUS_OK);
    assert(umi_desktop_panel_visibility_set_detail(&value, "Framework-owned control state") == UMI_STATUS_OK);
    assert(umi_desktop_panel_visibility_set_state(&value, 2U) == UMI_STATUS_OK);
    assert(umi_desktop_panel_visibility_set_priority(&value, 50U) == UMI_STATUS_OK);
    copy = value;
    assert(umi_desktop_panel_visibility_same_identity(&value, &copy));
    assert(strcmp(value.subject_id, "desk.subject") == 0);
    assert(umi_desktop_panel_visibility_effective(true, true));
    return 0;
}
