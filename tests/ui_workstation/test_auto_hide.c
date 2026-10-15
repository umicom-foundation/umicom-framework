/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_auto_hide.c
 *
 * PURPOSE:
 *   Implement the test auto hide behavior for
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
#include "umicom/ui/workstation/auto_hide.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted auto hide value. */
static int UmiWsAutoHideStateConstructionCases(void)
{
    UmiWsAutoHideState *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_ws_auto_hide_init(value, "outline", UMI_WS_DOCK_LEFT) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->surface_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_ws_auto_hide_init(value, oversized, UMI_WS_DOCK_LEFT) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_ws_auto_hide_init(value, value->surface_id, UMI_WS_DOCK_LEFT) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->surface_id, "outline") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiWsAutoHideStateConstructionCases() != 0) return 1;

    UmiWsAutoHideState s;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_auto_hide_init(&s, "outline", UMI_WS_DOCK_LEFT) != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_auto_hide_set_revealed(&s, true) != UMI_STATUS_OK || !s.revealed) return 2;
    /* Apply this operation only while the related capability or state is available. */
    if (umi_ws_auto_hide_edge_valid(UMI_WS_DOCK_CENTRE)) return 3;
    puts("auto hide: ok");
    return 0;
}
