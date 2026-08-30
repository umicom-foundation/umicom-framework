/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_control/test_command_session.c
 * PURPOSE: Validate the Framework-owned command session contract.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/desktop/control/command_session.h"

int main(void)
{
    UmiDesktopCommandSessionSnapshot value;
    UmiDesktopCommandSessionSnapshot copy;
    umi_desktop_command_session_init(&value, "command_session.primary");
    assert(umi_desktop_command_session_validate(&value) == UMI_STATUS_OK);
    assert(umi_desktop_command_session_set_subject(&value, "desk.subject") == UMI_STATUS_OK);
    assert(umi_desktop_command_session_set_detail(&value, "Framework-owned control state") == UMI_STATUS_OK);
    assert(umi_desktop_command_session_set_state(&value, 2U) == UMI_STATUS_OK);
    assert(umi_desktop_command_session_set_priority(&value, 50U) == UMI_STATUS_OK);
    copy = value;
    assert(umi_desktop_command_session_same_identity(&value, &copy));
    assert(strcmp(value.subject_id, "desk.subject") == 0);
    return 0;
}
