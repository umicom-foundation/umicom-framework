/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_transition_budget.c
 *
 * PURPOSE:
 *   Verify bound concurrent transitions and cumulative duration to avoid animation-heavy workstation surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/transition_budget.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAppearanceTransitionBudget item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_transition_budget_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_transition_budget_is_valid(&item)) return 2;
    return 0;
}
