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
#include <string.h>
#include "umicom/ui/workstation/tab_stack.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
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
