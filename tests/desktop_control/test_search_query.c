/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_control/test_search_query.c
 * PURPOSE: Validate the Framework-owned search query contract.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/desktop/control/search_query.h"

int main(void)
{
    UmiDesktopSearchQuerySnapshot value;
    UmiDesktopSearchQuerySnapshot copy;
    umi_desktop_search_query_init(&value, "search_query.primary");
    assert(umi_desktop_search_query_validate(&value) == UMI_STATUS_OK);
    assert(umi_desktop_search_query_set_subject(&value, "desk.subject") == UMI_STATUS_OK);
    assert(umi_desktop_search_query_set_detail(&value, "Framework-owned control state") == UMI_STATUS_OK);
    assert(umi_desktop_search_query_set_state(&value, 2U) == UMI_STATUS_OK);
    assert(umi_desktop_search_query_set_priority(&value, 50U) == UMI_STATUS_OK);
    copy = value;
    assert(umi_desktop_search_query_same_identity(&value, &copy));
    assert(strcmp(value.subject_id, "desk.subject") == 0);
    assert(umi_desktop_search_query_empty(""));
    return 0;
}
