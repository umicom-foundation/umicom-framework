/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/animation_gate.h
 *
 * PURPOSE:
 *   Decide whether an animation may run after reduced-motion and essential-feedback policy is applied.
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
#ifndef UMICOM_UI_APPEARANCE_ANIMATION_GATE_H
#define UMICOM_UI_APPEARANCE_ANIMATION_GATE_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance animation gate data shared with callers of this public
 * contract.
 */
typedef struct UmiAppearanceAnimationGate {
    char animation_id[UMI_APPEARANCE_ID_CAPACITY];
    bool essential;
    bool reduced_motion;
    bool allowed;
} UmiAppearanceAnimationGate;

/* Initialise one animation gate record with deterministic defaults. */
UmiStatus umi_appearance_animation_gate_init(UmiAppearanceAnimationGate *item);
/* Validate the required production invariants for this animation gate. */
int umi_appearance_animation_gate_is_valid(const UmiAppearanceAnimationGate *item);
/* Recalculate animation permission from motion accessibility state. */
void umi_appearance_animation_gate_resolve(UmiAppearanceAnimationGate *item);

#ifdef __cplusplus
}
#endif
#endif
