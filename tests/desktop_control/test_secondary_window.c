/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_control/test_secondary_window.c
 * PURPOSE: Validate the Framework-owned secondary window contract.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/desktop/control/secondary_window.h"

int main(void)
{
    UmiDesktopSecondaryWindowSnapshot value;
    UmiDesktopSecondaryWindowSnapshot copy;
    umi_desktop_secondary_window_init(&value, "secondary_window.primary");
    assert(umi_desktop_secondary_window_validate(&value) == UMI_STATUS_OK);
    assert(umi_desktop_secondary_window_set_subject(&value, "desk.subject") == UMI_STATUS_OK);
    assert(umi_desktop_secondary_window_set_detail(&value, "Framework-owned control state") == UMI_STATUS_OK);
    assert(umi_desktop_secondary_window_set_state(&value, 2U) == UMI_STATUS_OK);
    assert(umi_desktop_secondary_window_set_priority(&value, 50U) == UMI_STATUS_OK);
    copy = value;
    assert(umi_desktop_secondary_window_same_identity(&value, &copy));
    assert(strcmp(value.subject_id, "desk.subject") == 0);
    return 0;
}
