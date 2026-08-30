/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_control/test_application_placeholder.c
 * PURPOSE: Validate the Framework-owned application placeholder contract.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/desktop/control/application_placeholder.h"

int main(void)
{
    UmiDesktopApplicationPlaceholderSnapshot value;
    UmiDesktopApplicationPlaceholderSnapshot copy;
    umi_desktop_application_placeholder_init(&value, "application_placeholder.primary");
    assert(umi_desktop_application_placeholder_validate(&value) == UMI_STATUS_OK);
    assert(umi_desktop_application_placeholder_set_subject(&value, "desk.subject") == UMI_STATUS_OK);
    assert(umi_desktop_application_placeholder_set_detail(&value, "Framework-owned control state") == UMI_STATUS_OK);
    assert(umi_desktop_application_placeholder_set_state(&value, 2U) == UMI_STATUS_OK);
    assert(umi_desktop_application_placeholder_set_priority(&value, 50U) == UMI_STATUS_OK);
    copy = value;
    assert(umi_desktop_application_placeholder_same_identity(&value, &copy));
    assert(strcmp(value.subject_id, "desk.subject") == 0);
    assert(umi_desktop_application_placeholder_required(UMI_DESKTOP_CONTROL_AVAILABILITY_UNAVAILABLE, true));
    return 0;
}
