/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/line_height_policy.h
 *
 * PURPOSE:
 *   Maintain readable line-height bounds as font and accessibility scale changes.
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
#ifndef UMICOM_UI_APPEARANCE_LINE_HEIGHT_POLICY_H
#define UMICOM_UI_APPEARANCE_LINE_HEIGHT_POLICY_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAppearanceLineHeightPolicy {
    char policy_id[UMI_APPEARANCE_ID_CAPACITY];
    double minimum_multiplier;
    double preferred_multiplier;
    double maximum_multiplier;
} UmiAppearanceLineHeightPolicy;

/* Initialise one line height policy record with deterministic defaults. */
UmiStatus umi_appearance_line_height_policy_init(UmiAppearanceLineHeightPolicy *item);
/* Validate the required production invariants for this line height policy. */
int umi_appearance_line_height_policy_is_valid(const UmiAppearanceLineHeightPolicy *item);

#ifdef __cplusplus
}
#endif
#endif
