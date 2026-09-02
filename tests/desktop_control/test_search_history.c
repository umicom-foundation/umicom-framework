/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_control/test_search_history.c
 * PURPOSE: Validate the Framework-owned search history contract.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/desktop/control/search_history.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDesktopSearchHistorySnapshot value;
    UmiDesktopSearchHistorySnapshot copy;
    umi_desktop_search_history_init(&value, "search_history.primary");
    assert(umi_desktop_search_history_validate(&value) == UMI_STATUS_OK);
    assert(umi_desktop_search_history_set_subject(&value, "desk.subject") == UMI_STATUS_OK);
    assert(umi_desktop_search_history_set_detail(&value, "Framework-owned control state") == UMI_STATUS_OK);
    assert(umi_desktop_search_history_set_state(&value, 2U) == UMI_STATUS_OK);
    assert(umi_desktop_search_history_set_priority(&value, 50U) == UMI_STATUS_OK);
    copy = value;
    assert(umi_desktop_search_history_same_identity(&value, &copy));
    assert(strcmp(value.subject_id, "desk.subject") == 0);
    return 0;
}
