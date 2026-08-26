/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/high_dpi_policy.h
 *
 * PURPOSE:
 *   Define fractional-layout, snapping and asset-resolution rules for high-DPI displays.
 *
 * ARCHITECTURE:
 *   This production appearance capability extends canonical Umicom::ui and
 *   composes the existing Design System, adaptive shell and renderer contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_APPEARANCE_HIGH_DPI_POLICY_H
#define UMICOM_UI_APPEARANCE_HIGH_DPI_POLICY_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAppearanceHighDpiPolicy {
    char policy_id[UMI_APPEARANCE_ID_CAPACITY];
    bool allow_fractional_layout;
    bool snap_hairlines;
    bool prefer_vector_icons;
    uint32_t maximum_raster_scale;
} UmiAppearanceHighDpiPolicy;

/* Initialise one high dpi policy record with deterministic defaults. */
UmiStatus umi_appearance_high_dpi_policy_init(UmiAppearanceHighDpiPolicy *item);
/* Validate the required production invariants for this high dpi policy. */
int umi_appearance_high_dpi_policy_is_valid(const UmiAppearanceHighDpiPolicy *item);

#ifdef __cplusplus
}
#endif
#endif
