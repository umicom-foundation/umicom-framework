/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_control/test_search_result.c
 * PURPOSE: Validate the Framework-owned search result contract.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/desktop/control/search_result.h"

int main(void)
{
    UmiDesktopSearchResultSnapshot value;
    UmiDesktopSearchResultSnapshot copy;
    umi_desktop_search_result_init(&value, "search_result.primary");
    assert(umi_desktop_search_result_validate(&value) == UMI_STATUS_OK);
    assert(umi_desktop_search_result_set_subject(&value, "desk.subject") == UMI_STATUS_OK);
    assert(umi_desktop_search_result_set_detail(&value, "Framework-owned control state") == UMI_STATUS_OK);
    assert(umi_desktop_search_result_set_state(&value, 2U) == UMI_STATUS_OK);
    assert(umi_desktop_search_result_set_priority(&value, 50U) == UMI_STATUS_OK);
    copy = value;
    assert(umi_desktop_search_result_same_identity(&value, &copy));
    assert(strcmp(value.subject_id, "desk.subject") == 0);
    return 0;
}
