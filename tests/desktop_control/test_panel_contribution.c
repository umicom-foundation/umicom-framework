/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_control/test_panel_contribution.c
 * PURPOSE: Validate the Framework-owned panel contribution contract.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/desktop/control/panel_contribution.h"

int main(void)
{
    UmiDesktopPanelContributionSnapshot value;
    UmiDesktopPanelContributionSnapshot copy;
    umi_desktop_panel_contribution_init(&value, "panel_contribution.primary");
    assert(umi_desktop_panel_contribution_validate(&value) == UMI_STATUS_OK);
    assert(umi_desktop_panel_contribution_set_subject(&value, "desk.subject") == UMI_STATUS_OK);
    assert(umi_desktop_panel_contribution_set_detail(&value, "Framework-owned control state") == UMI_STATUS_OK);
    assert(umi_desktop_panel_contribution_set_state(&value, 2U) == UMI_STATUS_OK);
    assert(umi_desktop_panel_contribution_set_priority(&value, 50U) == UMI_STATUS_OK);
    copy = value;
    assert(umi_desktop_panel_contribution_same_identity(&value, &copy));
    assert(strcmp(value.subject_id, "desk.subject") == 0);
    return 0;
}
