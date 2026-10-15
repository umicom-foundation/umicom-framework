/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_panel_state.c
 *
 * PURPOSE:
 *   Implement the test panel state behavior for
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
#include "../value_archive/construction_cases.h"
#include "umicom/ui/workstation/panel_state.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted panel state value. */
static int UmiWsPanelStateConstructionCases(void)
{
    UmiWsPanelState *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_ws_panel_state_init(value, "ai.chat", UMI_WS_DOCK_RIGHT) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->surface_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_ws_panel_state_init(value, oversized, UMI_WS_DOCK_RIGHT) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_ws_panel_state_init(value, value->surface_id, UMI_WS_DOCK_RIGHT) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->surface_id, "ai.chat") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiWsPanelStateConstructionCases() != 0) return 1;

    UmiWsPanelState s;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_panel_state_init(&s, "ai.chat", UMI_WS_DOCK_RIGHT) != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_panel_state_set_mode(&s, UMI_WS_PANEL_AUTO_HIDDEN) != UMI_STATUS_OK) return 2;
    /* Apply this branch only when its contract condition is satisfied. */
    if (s.visibility != UMI_WS_VISIBILITY_AUTO_HIDE) return 3;
    umi_ws_panel_state_set_active(&s, true);
    /* Apply this operation only while the related capability or state is available. */
    if (!s.active) return 4;
    puts("panel state: ok");
    return 0;
}
