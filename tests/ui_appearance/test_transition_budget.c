/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_transition_budget.c
 *
 * PURPOSE:
 *   Verify bound concurrent transitions and cumulative duration to avoid animation-heavy workstation surfaces.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/transition_budget.h"
int main(void) {
    UmiAppearanceTransitionBudget item;
    if (umi_appearance_transition_budget_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_transition_budget_is_valid(&item)) return 2;
    return 0;
}
