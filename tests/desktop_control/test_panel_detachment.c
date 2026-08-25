/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_control/test_panel_detachment.c
 * PURPOSE: Validate the Framework-owned panel detachment contract.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/desktop/control/panel_detachment.h"

int main(void)
{
    UmiDesktopPanelDetachmentSnapshot value;
    UmiDesktopPanelDetachmentSnapshot copy;
    umi_desktop_panel_detachment_init(&value, "panel_detachment.primary");
    assert(umi_desktop_panel_detachment_validate(&value) == UMI_STATUS_OK);
    assert(umi_desktop_panel_detachment_set_subject(&value, "desk.subject") == UMI_STATUS_OK);
    assert(umi_desktop_panel_detachment_set_detail(&value, "Framework-owned control state") == UMI_STATUS_OK);
    assert(umi_desktop_panel_detachment_set_state(&value, 2U) == UMI_STATUS_OK);
    assert(umi_desktop_panel_detachment_set_priority(&value, 50U) == UMI_STATUS_OK);
    copy = value;
    assert(umi_desktop_panel_detachment_same_identity(&value, &copy));
    assert(strcmp(value.subject_id, "desk.subject") == 0);
    return 0;
}
