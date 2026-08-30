/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_control/test_context_direction.c
 * PURPOSE: Validate the Framework-owned context direction contract.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/desktop/control/context_direction.h"

int main(void)
{
    UmiDesktopContextDirectionSnapshot value;
    UmiDesktopContextDirectionSnapshot copy;
    umi_desktop_context_direction_init(&value, "context_direction.primary");
    assert(umi_desktop_context_direction_validate(&value) == UMI_STATUS_OK);
    assert(umi_desktop_context_direction_set_subject(&value, "desk.subject") == UMI_STATUS_OK);
    assert(umi_desktop_context_direction_set_detail(&value, "Framework-owned control state") == UMI_STATUS_OK);
    assert(umi_desktop_context_direction_set_state(&value, 2U) == UMI_STATUS_OK);
    assert(umi_desktop_context_direction_set_priority(&value, 50U) == UMI_STATUS_OK);
    copy = value;
    assert(umi_desktop_context_direction_same_identity(&value, &copy));
    assert(strcmp(value.subject_id, "desk.subject") == 0);
    assert(umi_desktop_context_direction_allows_publish(UMI_DESKTOP_CONTROL_DIRECTION_BIDIRECTIONAL));
    return 0;
}
