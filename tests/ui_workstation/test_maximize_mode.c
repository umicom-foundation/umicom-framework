/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_maximize_mode.c
 *
 * PURPOSE:
 *   Implement the test maximize mode behavior for
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
#include <string.h>
#include "umicom/ui/workstation/maximize_mode.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiWsMaximizeMode m;
    UmiWsMaximizeMode saved;
    char invalid[UMI_UI_ID_CAPACITY];
    char longest[UMI_UI_ID_CAPACITY];
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_maximize_mode_activate(&m, "chart") != UMI_STATUS_OK) return 1;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!umi_ws_maximize_mode_matches(&m, "chart")) return 2;
    /* Aliased reactivation preserves the identity instead of erasing it. */
    if (umi_ws_maximize_mode_activate(&m, m.surface_id) != UMI_STATUS_OK ||
        !umi_ws_maximize_mode_matches(&m, "chart")) return 4;
    (void)memcpy(&saved, &m, sizeof(saved));
    (void)memset(invalid, 'x', sizeof(invalid));
    if (umi_ws_maximize_mode_activate(&m, NULL) != UMI_STATUS_INVALID_ARGUMENT ||
        umi_ws_maximize_mode_activate(&m, "") != UMI_STATUS_INVALID_ARGUMENT ||
        umi_ws_maximize_mode_activate(&m, invalid) != UMI_STATUS_INVALID_ARGUMENT ||
        memcmp(&saved, &m, sizeof(m)) != 0) return 5;
    if (umi_ws_maximize_mode_activate(NULL, "chart") != UMI_STATUS_INVALID_ARGUMENT ||
        umi_ws_maximize_mode_matches(NULL, "chart") ||
        umi_ws_maximize_mode_matches(&m, invalid)) return 6;
    (void)memset(m.surface_id, 'x', sizeof(m.surface_id));
    if (umi_ws_maximize_mode_matches(&m, "chart")) return 7;
    (void)memset(longest, 'x', sizeof(longest));
    longest[sizeof(longest) - 1U] = '\0';
    if (umi_ws_maximize_mode_activate(&m, longest) != UMI_STATUS_OK ||
        !umi_ws_maximize_mode_matches(&m, longest)) return 8;
    umi_ws_maximize_mode_restore(&m);
    /* Apply this operation only while the related capability or state is available. */
    if (m.active) return 3;
    umi_ws_maximize_mode_restore(&m);
    umi_ws_maximize_mode_restore(NULL);
    if (umi_ws_maximize_mode_matches(&m, longest)) return 9;
    puts("maximize mode: ok");
    return 0;
}
