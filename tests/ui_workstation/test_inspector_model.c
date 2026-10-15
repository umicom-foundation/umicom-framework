/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_inspector_model.c
 *
 * PURPOSE:
 *   Implement the test inspector model behavior for
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
#include "umicom/ui/workstation/inspector_model.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted inspector model value. */
static int UmiWsInspectorModelConstructionCases(void)
{
    UmiWsInspectorModel *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_ws_inspector_model_init(value, "selected.widget") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->subject_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_ws_inspector_model_init(value, oversized) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_ws_inspector_model_init(value, value->subject_id) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->subject_id, "selected.widget") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiWsInspectorModelConstructionCases() != 0) return 1;

    UmiWsInspectorModel m;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_inspector_model_init(&m, "selected.widget") != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_inspector_model_set(&m, "width", "Width", "320", true) != UMI_STATUS_OK) return 2;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_ws_inspector_model_find(&m, "width") == NULL) return 3;
    puts("inspector model: ok");
    return 0;
}
