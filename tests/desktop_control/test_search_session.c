/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_control/test_search_session.c
 * PURPOSE: Validate the Framework-owned search session contract.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/desktop/control/search_session.h"

int main(void)
{
    UmiDesktopSearchSessionSnapshot value;
    UmiDesktopSearchSessionSnapshot copy;
    umi_desktop_search_session_init(&value, "search_session.primary");
    assert(umi_desktop_search_session_validate(&value) == UMI_STATUS_OK);
    assert(umi_desktop_search_session_set_subject(&value, "desk.subject") == UMI_STATUS_OK);
    assert(umi_desktop_search_session_set_detail(&value, "Framework-owned control state") == UMI_STATUS_OK);
    assert(umi_desktop_search_session_set_state(&value, 2U) == UMI_STATUS_OK);
    assert(umi_desktop_search_session_set_priority(&value, 50U) == UMI_STATUS_OK);
    copy = value;
    assert(umi_desktop_search_session_same_identity(&value, &copy));
    assert(strcmp(value.subject_id, "desk.subject") == 0);
    return 0;
}
