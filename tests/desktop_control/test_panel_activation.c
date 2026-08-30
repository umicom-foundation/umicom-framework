/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_control/test_panel_activation.c
 * PURPOSE: Validate the Framework-owned panel activation contract.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/desktop/control/panel_activation.h"

int main(void)
{
    UmiDesktopPanelActivationSnapshot value;
    UmiDesktopPanelActivationSnapshot copy;
    umi_desktop_panel_activation_init(&value, "panel_activation.primary");
    assert(umi_desktop_panel_activation_validate(&value) == UMI_STATUS_OK);
    assert(umi_desktop_panel_activation_set_subject(&value, "desk.subject") == UMI_STATUS_OK);
    assert(umi_desktop_panel_activation_set_detail(&value, "Framework-owned control state") == UMI_STATUS_OK);
    assert(umi_desktop_panel_activation_set_state(&value, 2U) == UMI_STATUS_OK);
    assert(umi_desktop_panel_activation_set_priority(&value, 50U) == UMI_STATUS_OK);
    copy = value;
    assert(umi_desktop_panel_activation_same_identity(&value, &copy));
    assert(strcmp(value.subject_id, "desk.subject") == 0);
    return 0;
}
