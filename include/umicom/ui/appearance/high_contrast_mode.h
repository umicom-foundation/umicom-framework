/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/high_contrast_mode.h
 *
 * PURPOSE:
 *   Represent high-contrast presentation requirements layered over the canonical Design System.
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
#ifndef UMICOM_UI_APPEARANCE_HIGH_CONTRAST_MODE_H
#define UMICOM_UI_APPEARANCE_HIGH_CONTRAST_MODE_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAppearanceHighContrastMode {
    char mode_id[UMI_APPEARANCE_ID_CAPACITY];
    bool enabled;
    bool force_visible_borders;
    bool force_focus_outline;
    double minimum_border_width;
} UmiAppearanceHighContrastMode;

/* Initialise one high contrast mode record with deterministic defaults. */
UmiStatus umi_appearance_high_contrast_mode_init(UmiAppearanceHighContrastMode *item);
/* Validate the required production invariants for this high contrast mode. */
int umi_appearance_high_contrast_mode_is_valid(const UmiAppearanceHighContrastMode *item);

#ifdef __cplusplus
}
#endif
#endif
