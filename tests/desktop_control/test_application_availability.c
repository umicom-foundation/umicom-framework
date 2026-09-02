/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_control/test_application_availability.c
 * PURPOSE: Validate the Framework-owned application availability contract.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/desktop/control/application_availability.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDesktopApplicationAvailabilitySnapshot value;
    UmiDesktopApplicationAvailabilitySnapshot copy;
    umi_desktop_application_availability_init(&value, "application_availability.primary");
    assert(umi_desktop_application_availability_validate(&value) == UMI_STATUS_OK);
    assert(umi_desktop_application_availability_set_subject(&value, "desk.subject") == UMI_STATUS_OK);
    assert(umi_desktop_application_availability_set_detail(&value, "Framework-owned control state") == UMI_STATUS_OK);
    assert(umi_desktop_application_availability_set_state(&value, 2U) == UMI_STATUS_OK);
    assert(umi_desktop_application_availability_set_priority(&value, 50U) == UMI_STATUS_OK);
    copy = value;
    assert(umi_desktop_application_availability_same_identity(&value, &copy));
    assert(strcmp(value.subject_id, "desk.subject") == 0);
    assert(umi_desktop_application_availability_launchable(UMI_DESKTOP_CONTROL_AVAILABILITY_AVAILABLE));
    return 0;
}
