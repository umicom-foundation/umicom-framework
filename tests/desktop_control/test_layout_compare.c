/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_control/test_layout_compare.c
 * PURPOSE: Validate the Framework-owned layout compare contract.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/desktop/control/layout_compare.h"

int main(void)
{
    UmiDesktopLayoutCompareSnapshot value;
    UmiDesktopLayoutCompareSnapshot copy;
    umi_desktop_layout_compare_init(&value, "layout_compare.primary");
    assert(umi_desktop_layout_compare_validate(&value) == UMI_STATUS_OK);
    assert(umi_desktop_layout_compare_set_subject(&value, "desk.subject") == UMI_STATUS_OK);
    assert(umi_desktop_layout_compare_set_detail(&value, "Framework-owned control state") == UMI_STATUS_OK);
    assert(umi_desktop_layout_compare_set_state(&value, 2U) == UMI_STATUS_OK);
    assert(umi_desktop_layout_compare_set_priority(&value, 50U) == UMI_STATUS_OK);
    copy = value;
    assert(umi_desktop_layout_compare_same_identity(&value, &copy));
    assert(strcmp(value.subject_id, "desk.subject") == 0);
    assert(umi_desktop_layout_compare_fingerprints_equal(7U, 7U));
    return 0;
}
