/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/transition_budget.h
 *
 * PURPOSE:
 *   Bound concurrent transitions and cumulative duration to avoid animation-heavy workstation surfaces.
 *
 * ARCHITECTURE:
 *   This production appearance capability extends canonical Umicom::ui and
 *   composes the existing Design System, adaptive shell and renderer contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_APPEARANCE_TRANSITION_BUDGET_H
#define UMICOM_UI_APPEARANCE_TRANSITION_BUDGET_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAppearanceTransitionBudget {
    char budget_id[UMI_APPEARANCE_ID_CAPACITY];
    uint32_t max_concurrent;
    uint32_t max_duration_ms;
    uint32_t max_delayed_ms;
} UmiAppearanceTransitionBudget;

/* Initialise one transition budget record with deterministic defaults. */
UmiStatus umi_appearance_transition_budget_init(UmiAppearanceTransitionBudget *item);
/* Validate the required production invariants for this transition budget. */
int umi_appearance_transition_budget_is_valid(const UmiAppearanceTransitionBudget *item);

#ifdef __cplusplus
}
#endif
#endif
