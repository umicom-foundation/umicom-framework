/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_control/test_types.c
 * PURPOSE: Validate shared Desk control helpers.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/desktop/control/types.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    char text[16];
    UmiDesktopControlRect bounds = {0, 0, 100, 100};
    UmiDesktopControlRect rect = {-5, 80, 50, 50};
    assert(umi_desktop_control_copy_text(text, sizeof(text), "desk") == UMI_STATUS_OK);
    assert(strcmp(text, "desk") == 0);
    assert(umi_desktop_control_id_valid("desk.panel"));
    assert(umi_desktop_control_text_contains("Umicom Studio", "studio"));
    assert(umi_desktop_control_rect_clamp(&rect, &bounds) == UMI_STATUS_OK);
    assert(rect.x == 0 && rect.y == 50);
    assert(umi_desktop_control_hash_text("desk") != 0U);
    return 0;
}
