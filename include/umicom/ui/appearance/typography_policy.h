/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/typography_policy.h
 *
 * PURPOSE:
 *   Govern semantic typography scaling, minimum readable text size and font smoothing intent.
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
#ifndef UMICOM_UI_APPEARANCE_TYPOGRAPHY_POLICY_H
#define UMICOM_UI_APPEARANCE_TYPOGRAPHY_POLICY_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance typography policy data shared with callers of this public
 * contract.
 */
typedef struct UmiAppearanceTypographyPolicy {
    char policy_id[UMI_APPEARANCE_ID_CAPACITY];
    double base_text_scale;
    double minimum_text_dp;
    double maximum_text_scale;
    bool respect_user_scale;
} UmiAppearanceTypographyPolicy;

/* Initialise one typography policy record with deterministic defaults. */
UmiStatus umi_appearance_typography_policy_init(UmiAppearanceTypographyPolicy *item);
/* Validate the required production invariants for this typography policy. */
int umi_appearance_typography_policy_is_valid(const UmiAppearanceTypographyPolicy *item);

#ifdef __cplusplus
}
#endif
#endif
