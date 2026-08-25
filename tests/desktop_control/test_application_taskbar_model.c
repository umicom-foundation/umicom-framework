/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_control/test_application_taskbar_model.c
 * PURPOSE: Validate the Framework-owned application taskbar model contract.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/desktop/control/application_taskbar_model.h"

int main(void)
{
    UmiDesktopApplicationTaskbarModelSnapshot value;
    UmiDesktopApplicationTaskbarModelSnapshot copy;
    umi_desktop_application_taskbar_model_init(&value, "application_taskbar_model.primary");
    assert(umi_desktop_application_taskbar_model_validate(&value) == UMI_STATUS_OK);
    assert(umi_desktop_application_taskbar_model_set_subject(&value, "desk.subject") == UMI_STATUS_OK);
    assert(umi_desktop_application_taskbar_model_set_detail(&value, "Framework-owned control state") == UMI_STATUS_OK);
    assert(umi_desktop_application_taskbar_model_set_state(&value, 2U) == UMI_STATUS_OK);
    assert(umi_desktop_application_taskbar_model_set_priority(&value, 50U) == UMI_STATUS_OK);
    copy = value;
    assert(umi_desktop_application_taskbar_model_same_identity(&value, &copy));
    assert(strcmp(value.subject_id, "desk.subject") == 0);
    return 0;
}
