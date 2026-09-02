/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/color_vision_assistance.h
 *
 * PURPOSE:
 *   Require non-colour cues when semantic state could otherwise depend on colour perception alone.
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
#ifndef UMICOM_UI_APPEARANCE_COLOR_VISION_ASSISTANCE_H
#define UMICOM_UI_APPEARANCE_COLOR_VISION_ASSISTANCE_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance color vision assistance data shared with callers of this public
 * contract.
 */
typedef struct UmiAppearanceColorVisionAssistance {
    char profile_id[UMI_APPEARANCE_ID_CAPACITY];
    bool require_icons;
    bool require_text_labels;
    bool require_patterns;
    bool avoid_red_green_only;
} UmiAppearanceColorVisionAssistance;

/* Initialise one color vision assistance record with deterministic defaults. */
UmiStatus umi_appearance_color_vision_assistance_init(UmiAppearanceColorVisionAssistance *item);
/* Validate the required production invariants for this color vision assistance. */
int umi_appearance_color_vision_assistance_is_valid(const UmiAppearanceColorVisionAssistance *item);

#ifdef __cplusplus
}
#endif
#endif
