/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_control/test_application_lifecycle.c
 * PURPOSE: Validate the Framework-owned application lifecycle contract.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/desktop/control/application_lifecycle.h"

int main(void)
{
    UmiDesktopApplicationLifecycleSnapshot value;
    UmiDesktopApplicationLifecycleSnapshot copy;
    umi_desktop_application_lifecycle_init(&value, "application_lifecycle.primary");
    assert(umi_desktop_application_lifecycle_validate(&value) == UMI_STATUS_OK);
    assert(umi_desktop_application_lifecycle_set_subject(&value, "desk.subject") == UMI_STATUS_OK);
    assert(umi_desktop_application_lifecycle_set_detail(&value, "Framework-owned control state") == UMI_STATUS_OK);
    assert(umi_desktop_application_lifecycle_set_state(&value, 2U) == UMI_STATUS_OK);
    assert(umi_desktop_application_lifecycle_set_priority(&value, 50U) == UMI_STATUS_OK);
    copy = value;
    assert(umi_desktop_application_lifecycle_same_identity(&value, &copy));
    assert(strcmp(value.subject_id, "desk.subject") == 0);
    assert(umi_desktop_application_lifecycle_transition_allowed(UMI_DESKTOP_CONTROL_LIFECYCLE_STOPPED, UMI_DESKTOP_CONTROL_LIFECYCLE_STARTING));
    return 0;
}
