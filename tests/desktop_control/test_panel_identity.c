/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_control/test_panel_identity.c
 * PURPOSE: Validate the Framework-owned panel identity contract.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/desktop/control/panel_identity.h"

int main(void)
{
    UmiDesktopPanelIdentitySnapshot value;
    UmiDesktopPanelIdentitySnapshot copy;
    umi_desktop_panel_identity_init(&value, "panel_identity.primary");
    assert(umi_desktop_panel_identity_validate(&value) == UMI_STATUS_OK);
    assert(umi_desktop_panel_identity_set_subject(&value, "desk.subject") == UMI_STATUS_OK);
    assert(umi_desktop_panel_identity_set_detail(&value, "Framework-owned control state") == UMI_STATUS_OK);
    assert(umi_desktop_panel_identity_set_state(&value, 2U) == UMI_STATUS_OK);
    assert(umi_desktop_panel_identity_set_priority(&value, 50U) == UMI_STATUS_OK);
    copy = value;
    assert(umi_desktop_panel_identity_same_identity(&value, &copy));
    assert(strcmp(value.subject_id, "desk.subject") == 0);
    return 0;
}
