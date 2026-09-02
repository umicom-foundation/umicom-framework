/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/input_affordance_policy.h
 *
 * PURPOSE:
 *   Require hover, focus, pressed and touch feedback appropriate to available input modalities.
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
#ifndef UMICOM_UI_APPEARANCE_INPUT_AFFORDANCE_POLICY_H
#define UMICOM_UI_APPEARANCE_INPUT_AFFORDANCE_POLICY_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance input affordance policy data shared with callers of this public
 * contract.
 */
typedef struct UmiAppearanceInputAffordancePolicy {
    char policy_id[UMI_APPEARANCE_ID_CAPACITY];
    bool require_hover_feedback;
    bool require_focus_feedback;
    bool require_pressed_feedback;
    bool require_touch_feedback;
} UmiAppearanceInputAffordancePolicy;

/* Initialise one input affordance policy record with deterministic defaults. */
UmiStatus umi_appearance_input_affordance_policy_init(UmiAppearanceInputAffordancePolicy *item);
/* Validate the required production invariants for this input affordance policy. */
int umi_appearance_input_affordance_policy_is_valid(const UmiAppearanceInputAffordancePolicy *item);

#ifdef __cplusplus
}
#endif
#endif
