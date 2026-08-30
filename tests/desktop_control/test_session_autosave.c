/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_control/test_session_autosave.c
 * PURPOSE: Validate the Framework-owned session autosave contract.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/desktop/control/session_autosave.h"

int main(void)
{
    UmiDesktopSessionAutosaveSnapshot value;
    UmiDesktopSessionAutosaveSnapshot copy;
    umi_desktop_session_autosave_init(&value, "session_autosave.primary");
    assert(umi_desktop_session_autosave_validate(&value) == UMI_STATUS_OK);
    assert(umi_desktop_session_autosave_set_subject(&value, "desk.subject") == UMI_STATUS_OK);
    assert(umi_desktop_session_autosave_set_detail(&value, "Framework-owned control state") == UMI_STATUS_OK);
    assert(umi_desktop_session_autosave_set_state(&value, 2U) == UMI_STATUS_OK);
    assert(umi_desktop_session_autosave_set_priority(&value, 50U) == UMI_STATUS_OK);
    copy = value;
    assert(umi_desktop_session_autosave_same_identity(&value, &copy));
    assert(strcmp(value.subject_id, "desk.subject") == 0);
    assert(umi_desktop_session_autosave_due(2000U, 1000U, 500U, true));
    return 0;
}
