/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/contrast_policy.h
 *
 * PURPOSE:
 *   Define certification thresholds for normal text, large text, icons and focus indicators.
 *
 * ARCHITECTURE:
 *   This production appearance capability extends canonical Umicom::ui and
 *   composes the existing Design System, adaptive shell and renderer contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_APPEARANCE_CONTRAST_POLICY_H
#define UMICOM_UI_APPEARANCE_CONTRAST_POLICY_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAppearanceContrastPolicy {
    char policy_id[UMI_APPEARANCE_ID_CAPACITY];
    double normal_text_ratio;
    double large_text_ratio;
    double non_text_ratio;
    double focus_ratio;
} UmiAppearanceContrastPolicy;

/* Initialise one contrast policy record with deterministic defaults. */
UmiStatus umi_appearance_contrast_policy_init(UmiAppearanceContrastPolicy *item);
/* Validate the required production invariants for this contrast policy. */
int umi_appearance_contrast_policy_is_valid(const UmiAppearanceContrastPolicy *item);

#ifdef __cplusplus
}
#endif
#endif
