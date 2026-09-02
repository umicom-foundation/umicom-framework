/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/transition_budget.c
 *
 * PURPOSE:
 *   Bound concurrent transitions and cumulative duration to avoid animation-heavy workstation surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/transition_budget.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_transition_budget_init(UmiAppearanceTransitionBudget *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->budget_id,sizeof item->budget_id,"transition.workstation");
    item->max_concurrent=8U;
    item->max_duration_ms=300U;
    item->max_delayed_ms=150U;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_transition_budget_is_valid(const UmiAppearanceTransitionBudget *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->budget_id) && item->max_concurrent > 0U && item->max_duration_ms > 0U);
}
