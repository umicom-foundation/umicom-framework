/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/appearance_conformance.h
 *
 * PURPOSE:
 *   Define release-gate requirements for theme, high-DPI, accessibility and frontend appearance parity.
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
#ifndef UMICOM_UI_APPEARANCE_APPEARANCE_CONFORMANCE_H
#define UMICOM_UI_APPEARANCE_APPEARANCE_CONFORMANCE_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance appearance conformance data shared with callers of this public
 * contract.
 */
typedef struct UmiAppearanceAppearanceConformance {
    char profile_id[UMI_APPEARANCE_ID_CAPACITY];
    bool require_theme_parity;
    bool require_high_dpi;
    bool require_contrast;
    bool require_keyboard_focus;
    bool require_reduced_motion;
    bool passed;
} UmiAppearanceAppearanceConformance;

/* Initialise one appearance conformance record with deterministic defaults. */
UmiStatus umi_appearance_conformance_init(UmiAppearanceAppearanceConformance *item);
/* Validate the required production invariants for this appearance conformance. */
int umi_appearance_conformance_is_valid(const UmiAppearanceAppearanceConformance *item);

#ifdef __cplusplus
}
#endif
#endif
