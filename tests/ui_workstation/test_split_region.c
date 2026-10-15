/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_split_region.c
 *
 * PURPOSE:
 *   Implement the test split region behavior for
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
#include "umicom/ui/workstation/split_region.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted split region value. */
static int UmiWsSplitRegionConstructionCases(void)
{
    UmiWsSplitRegion *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_ws_split_region_init(value, "main.split", UMI_UI_HORIZONTAL, 0.7) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->split_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_ws_split_region_init(value, oversized, UMI_UI_HORIZONTAL, 0.7) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_ws_split_region_init(value, value->split_id, UMI_UI_HORIZONTAL, 0.7) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->split_id, "main.split") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiWsSplitRegionConstructionCases() != 0) return 1;

    UmiWsSplitRegion s;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_split_region_init(&s, "main.split", UMI_UI_HORIZONTAL, 0.7) != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_split_region_bind(&s, "left", "centre") != UMI_STATUS_OK) return 2;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ws_split_region_set_ratio(&s, 0.99) != 0.85) return 3;
    puts("split region: ok");
    return 0;
}
