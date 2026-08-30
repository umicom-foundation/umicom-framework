/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/input_target_policy.h
 *
 * PURPOSE:
 *   Resolve minimum interactive target dimensions by pointer, touch, keyboard or hybrid modality.
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
#ifndef UMICOM_UI_APPEARANCE_INPUT_TARGET_POLICY_H
#define UMICOM_UI_APPEARANCE_INPUT_TARGET_POLICY_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAppearanceInputTargetPolicy {
    char policy_id[UMI_APPEARANCE_ID_CAPACITY];
    UmiAppearanceInputModality modality;
    double minimum_width_dp;
    double minimum_height_dp;
} UmiAppearanceInputTargetPolicy;

/* Initialise one input target policy record with deterministic defaults. */
UmiStatus umi_appearance_input_target_policy_init(UmiAppearanceInputTargetPolicy *item);
/* Validate the required production invariants for this input target policy. */
int umi_appearance_input_target_policy_is_valid(const UmiAppearanceInputTargetPolicy *item);
/* Apply Framework baseline minimums for a requested input modality. */
UmiStatus umi_appearance_input_target_policy_for_modality(UmiAppearanceInputTargetPolicy *item,UmiAppearanceInputModality modality);

#ifdef __cplusplus
}
#endif
#endif
