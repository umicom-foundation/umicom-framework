/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/renderer_appearance_capability.h
 *
 * PURPOSE:
 *   Declare appearance capabilities and limitations for GTK4, Qt6, Native Web or headless renderers.
 *
 * ARCHITECTURE:
 *   This production appearance capability extends canonical Umicom::ui and
 *   composes the existing Design System, adaptive shell and renderer contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_APPEARANCE_RENDERER_APPEARANCE_CAPABILITY_H
#define UMICOM_UI_APPEARANCE_RENDERER_APPEARANCE_CAPABILITY_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAppearanceRendererAppearanceCapability {
    char renderer_id[UMI_APPEARANCE_ID_CAPACITY];
    UmiAppearanceRendererKind kind;
    bool supports_fractional_scale;
    bool supports_high_contrast;
    bool supports_reduced_motion;
    bool supports_symbolic_icons;
} UmiAppearanceRendererAppearanceCapability;

/* Initialise one renderer appearance capability record with deterministic defaults. */
UmiStatus umi_appearance_renderer_appearance_capability_init(UmiAppearanceRendererAppearanceCapability *item);
/* Validate the required production invariants for this renderer appearance capability. */
int umi_appearance_renderer_appearance_capability_is_valid(const UmiAppearanceRendererAppearanceCapability *item);

#ifdef __cplusplus
}
#endif
#endif
