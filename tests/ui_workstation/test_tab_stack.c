/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_tab_stack.c
 *
 * PURPOSE:
 *   Implement the test tab stack behavior for
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
#include <string.h>
#include "umicom/ui/workstation/tab_stack.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted tab stack value. */
static int UmiWsTabStackConstructionCases(void)
{
    UmiWsTabStack *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_ws_tab_stack_init(value, "editors") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->stack_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_ws_tab_stack_init(value, oversized) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_ws_tab_stack_init(value, value->stack_id) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->stack_id, "editors") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiWsTabStackConstructionCases() != 0) return 1;

    UmiWsTabStack s;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_tab_stack_init(&s, "editors") != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_tab_stack_add(&s, "editor.a") != UMI_STATUS_OK) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_tab_stack_add(&s, "editor.b") != UMI_STATUS_OK) return 3;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_tab_stack_activate(&s, "editor.b") != UMI_STATUS_OK) return 4;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(umi_ws_tab_stack_active(&s), "editor.b") != 0) return 5;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_tab_stack_remove(&s, "editor.a") != UMI_STATUS_OK) return 6;
    puts("tab stack: ok");
    return 0;
}
