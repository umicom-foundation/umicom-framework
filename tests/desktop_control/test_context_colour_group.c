/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_control/test_context_colour_group.c
 * PURPOSE: Validate the Framework-owned context colour group contract.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/desktop/control/context_colour_group.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDesktopContextColourGroupSnapshot value;
    UmiDesktopContextColourGroupSnapshot copy;
    umi_desktop_context_colour_group_init(&value, "context_colour_group.primary");
    assert(umi_desktop_context_colour_group_validate(&value) == UMI_STATUS_OK);
    assert(umi_desktop_context_colour_group_set_subject(&value, "desk.subject") == UMI_STATUS_OK);
    assert(umi_desktop_context_colour_group_set_detail(&value, "Framework-owned control state") == UMI_STATUS_OK);
    assert(umi_desktop_context_colour_group_set_state(&value, 2U) == UMI_STATUS_OK);
    assert(umi_desktop_context_colour_group_set_priority(&value, 50U) == UMI_STATUS_OK);
    copy = value;
    assert(umi_desktop_context_colour_group_same_identity(&value, &copy));
    assert(strcmp(value.subject_id, "desk.subject") == 0);
    return 0;
}
