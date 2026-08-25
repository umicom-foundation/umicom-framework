/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_control/test_layout_lock.c
 * PURPOSE: Validate the Framework-owned layout lock contract.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/desktop/control/layout_lock.h"

int main(void)
{
    UmiDesktopLayoutLockSnapshot value;
    UmiDesktopLayoutLockSnapshot copy;
    umi_desktop_layout_lock_init(&value, "layout_lock.primary");
    assert(umi_desktop_layout_lock_validate(&value) == UMI_STATUS_OK);
    assert(umi_desktop_layout_lock_set_subject(&value, "desk.subject") == UMI_STATUS_OK);
    assert(umi_desktop_layout_lock_set_detail(&value, "Framework-owned control state") == UMI_STATUS_OK);
    assert(umi_desktop_layout_lock_set_state(&value, 2U) == UMI_STATUS_OK);
    assert(umi_desktop_layout_lock_set_priority(&value, 50U) == UMI_STATUS_OK);
    copy = value;
    assert(umi_desktop_layout_lock_same_identity(&value, &copy));
    assert(strcmp(value.subject_id, "desk.subject") == 0);
    assert(umi_desktop_layout_lock_mutation_allowed(true, "owner", "owner"));
    return 0;
}
