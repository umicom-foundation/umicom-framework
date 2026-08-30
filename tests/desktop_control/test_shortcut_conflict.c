/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_control/test_shortcut_conflict.c
 * PURPOSE: Validate the Framework-owned shortcut conflict contract.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/desktop/control/shortcut_conflict.h"

int main(void)
{
    UmiDesktopShortcutConflictSnapshot value;
    UmiDesktopShortcutConflictSnapshot copy;
    umi_desktop_shortcut_conflict_init(&value, "shortcut_conflict.primary");
    assert(umi_desktop_shortcut_conflict_validate(&value) == UMI_STATUS_OK);
    assert(umi_desktop_shortcut_conflict_set_subject(&value, "desk.subject") == UMI_STATUS_OK);
    assert(umi_desktop_shortcut_conflict_set_detail(&value, "Framework-owned control state") == UMI_STATUS_OK);
    assert(umi_desktop_shortcut_conflict_set_state(&value, 2U) == UMI_STATUS_OK);
    assert(umi_desktop_shortcut_conflict_set_priority(&value, 50U) == UMI_STATUS_OK);
    copy = value;
    assert(umi_desktop_shortcut_conflict_same_identity(&value, &copy));
    assert(strcmp(value.subject_id, "desk.subject") == 0);
    assert(umi_desktop_shortcut_conflict_conflicts("Ctrl+P", "Ctrl+P", "desk", "desk"));
    return 0;
}
