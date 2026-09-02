/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_theme_role.c
 *
 * PURPOSE:
 *   Implement the test theme role behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include "umicom/ui/workstation/theme_role.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_theme_role_token(UMI_WS_THEME_ACCENT)[0] != 'w') return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_theme_role_css_class(UMI_WS_THEME_ERROR)[4] != 'e') return 2;
    puts("theme role: ok");
    return 0;
}
