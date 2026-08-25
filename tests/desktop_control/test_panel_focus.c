/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_control/test_panel_focus.c
 * PURPOSE: Validate the Framework-owned panel focus contract.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/desktop/control/panel_focus.h"

int main(void)
{
    UmiDesktopPanelFocusSnapshot value;
    UmiDesktopPanelFocusSnapshot copy;
    umi_desktop_panel_focus_init(&value, "panel_focus.primary");
    assert(umi_desktop_panel_focus_validate(&value) == UMI_STATUS_OK);
    assert(umi_desktop_panel_focus_set_subject(&value, "desk.subject") == UMI_STATUS_OK);
    assert(umi_desktop_panel_focus_set_detail(&value, "Framework-owned control state") == UMI_STATUS_OK);
    assert(umi_desktop_panel_focus_set_state(&value, 2U) == UMI_STATUS_OK);
    assert(umi_desktop_panel_focus_set_priority(&value, 50U) == UMI_STATUS_OK);
    copy = value;
    assert(umi_desktop_panel_focus_same_identity(&value, &copy));
    assert(strcmp(value.subject_id, "desk.subject") == 0);
    return 0;
}
