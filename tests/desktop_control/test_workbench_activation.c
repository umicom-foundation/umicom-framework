/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_control/test_workbench_activation.c
 * PURPOSE: Validate the Framework-owned workbench activation contract.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/desktop/control/workbench_activation.h"

int main(void)
{
    UmiDesktopWorkbenchActivationSnapshot value;
    UmiDesktopWorkbenchActivationSnapshot copy;
    umi_desktop_workbench_activation_init(&value, "workbench_activation.primary");
    assert(umi_desktop_workbench_activation_validate(&value) == UMI_STATUS_OK);
    assert(umi_desktop_workbench_activation_set_subject(&value, "desk.subject") == UMI_STATUS_OK);
    assert(umi_desktop_workbench_activation_set_detail(&value, "Framework-owned control state") == UMI_STATUS_OK);
    assert(umi_desktop_workbench_activation_set_state(&value, 2U) == UMI_STATUS_OK);
    assert(umi_desktop_workbench_activation_set_priority(&value, 50U) == UMI_STATUS_OK);
    copy = value;
    assert(umi_desktop_workbench_activation_same_identity(&value, &copy));
    assert(strcmp(value.subject_id, "desk.subject") == 0);
    return 0;
}
