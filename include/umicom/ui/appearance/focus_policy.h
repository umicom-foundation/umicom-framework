/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/focus_policy.h
 *
 * PURPOSE:
 *   Define visible keyboard-focus treatment requirements across all renderer adapters.
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
#ifndef UMICOM_UI_APPEARANCE_FOCUS_POLICY_H
#define UMICOM_UI_APPEARANCE_FOCUS_POLICY_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance focus policy data shared with callers of this public contract.
 */
typedef struct UmiAppearanceFocusPolicy {
    char policy_id[UMI_APPEARANCE_ID_CAPACITY];
    double ring_width;
    double ring_offset;
    bool always_visible_for_keyboard;
    bool clip_safe;
} UmiAppearanceFocusPolicy;

/* Initialise one focus policy record with deterministic defaults. */
UmiStatus umi_appearance_focus_policy_init(UmiAppearanceFocusPolicy *item);
/* Validate the required production invariants for this focus policy. */
int umi_appearance_focus_policy_is_valid(const UmiAppearanceFocusPolicy *item);

#ifdef __cplusplus
}
#endif
#endif
