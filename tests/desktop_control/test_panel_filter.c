/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_control/test_panel_filter.c
 * PURPOSE: Validate the Framework-owned panel filter contract.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/desktop/control/panel_filter.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDesktopPanelFilterSnapshot value;
    UmiDesktopPanelFilterSnapshot copy;
    umi_desktop_panel_filter_init(&value, "panel_filter.primary");
    assert(umi_desktop_panel_filter_validate(&value) == UMI_STATUS_OK);
    assert(umi_desktop_panel_filter_set_subject(&value, "desk.subject") == UMI_STATUS_OK);
    assert(umi_desktop_panel_filter_set_detail(&value, "Framework-owned control state") == UMI_STATUS_OK);
    assert(umi_desktop_panel_filter_set_state(&value, 2U) == UMI_STATUS_OK);
    assert(umi_desktop_panel_filter_set_priority(&value, 50U) == UMI_STATUS_OK);
    copy = value;
    assert(umi_desktop_panel_filter_same_identity(&value, &copy));
    assert(strcmp(value.subject_id, "desk.subject") == 0);
    assert(umi_desktop_panel_filter_matches("studio", "developer", "dev"));
    return 0;
}
