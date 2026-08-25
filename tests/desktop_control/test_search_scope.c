/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_control/test_search_scope.c
 * PURPOSE: Validate the Framework-owned search scope contract.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/desktop/control/search_scope.h"

int main(void)
{
    UmiDesktopSearchScopeSnapshot value;
    UmiDesktopSearchScopeSnapshot copy;
    umi_desktop_search_scope_init(&value, "search_scope.primary");
    assert(umi_desktop_search_scope_validate(&value) == UMI_STATUS_OK);
    assert(umi_desktop_search_scope_set_subject(&value, "desk.subject") == UMI_STATUS_OK);
    assert(umi_desktop_search_scope_set_detail(&value, "Framework-owned control state") == UMI_STATUS_OK);
    assert(umi_desktop_search_scope_set_state(&value, 2U) == UMI_STATUS_OK);
    assert(umi_desktop_search_scope_set_priority(&value, 50U) == UMI_STATUS_OK);
    copy = value;
    assert(umi_desktop_search_scope_same_identity(&value, &copy));
    assert(strcmp(value.subject_id, "desk.subject") == 0);
    return 0;
}
