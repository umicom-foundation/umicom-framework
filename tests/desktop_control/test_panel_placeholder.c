/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_control/test_panel_placeholder.c
 * PURPOSE: Validate the Framework-owned panel placeholder contract.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/desktop/control/panel_placeholder.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDesktopPanelPlaceholderSnapshot value;
    UmiDesktopPanelPlaceholderSnapshot copy;
    umi_desktop_panel_placeholder_init(&value, "panel_placeholder.primary");
    assert(umi_desktop_panel_placeholder_validate(&value) == UMI_STATUS_OK);
    assert(umi_desktop_panel_placeholder_set_subject(&value, "desk.subject") == UMI_STATUS_OK);
    assert(umi_desktop_panel_placeholder_set_detail(&value, "Framework-owned control state") == UMI_STATUS_OK);
    assert(umi_desktop_panel_placeholder_set_state(&value, 2U) == UMI_STATUS_OK);
    assert(umi_desktop_panel_placeholder_set_priority(&value, 50U) == UMI_STATUS_OK);
    copy = value;
    assert(umi_desktop_panel_placeholder_same_identity(&value, &copy));
    assert(strcmp(value.subject_id, "desk.subject") == 0);
    assert(umi_desktop_panel_placeholder_required(false, true));
    return 0;
}
