/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_control/test_search_source.c
 * PURPOSE: Validate the Framework-owned search source contract.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/desktop/control/search_source.h"

int main(void)
{
    UmiDesktopSearchSourceSnapshot value;
    UmiDesktopSearchSourceSnapshot copy;
    umi_desktop_search_source_init(&value, "search_source.primary");
    assert(umi_desktop_search_source_validate(&value) == UMI_STATUS_OK);
    assert(umi_desktop_search_source_set_subject(&value, "desk.subject") == UMI_STATUS_OK);
    assert(umi_desktop_search_source_set_detail(&value, "Framework-owned control state") == UMI_STATUS_OK);
    assert(umi_desktop_search_source_set_state(&value, 2U) == UMI_STATUS_OK);
    assert(umi_desktop_search_source_set_priority(&value, 50U) == UMI_STATUS_OK);
    copy = value;
    assert(umi_desktop_search_source_same_identity(&value, &copy));
    assert(strcmp(value.subject_id, "desk.subject") == 0);
    return 0;
}
