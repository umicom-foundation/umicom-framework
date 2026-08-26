/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/design_system.h
 *
 * PURPOSE:
 *   Aggregate typed tokens, palettes and semantic component descriptors into one canonical reusable design system.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_UI_DESIGN_DESIGN_SYSTEM_H
#define INCLUDE_UMICOM_UI_DESIGN_DESIGN_SYSTEM_H

#include "umicom/ui/design/types.h"
#include "umicom/ui/design/token_set.h"
#include "umicom/ui/design/color_palette.h"
#include "umicom/ui/design/component_catalogue.h"
#include "umicom/ui/design/density.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/ui/design/token_set.h"
#include "umicom/ui/design/color_palette.h"
#include "umicom/ui/design/component_catalogue.h"
#include "umicom/ui/design/density.h"
typedef struct UmiDesignSystem { char id[UMI_DESIGN_ID_CAPACITY]; UmiDesignThemeMode default_theme; UmiDesignDensity default_density; UmiDesignTokenSet tokens; UmiDesignColorPalette palette; UmiDesignComponentCatalogue components; uint64_t revision; } UmiDesignSystem;
/* Initialise a named design system with explicit default theme and density. */
UmiStatus umi_design_system_init(UmiDesignSystem *system, const char *id, UmiDesignThemeMode theme, UmiDesignDensity density);
/* Return one when required design-system identity and defaults are valid. */
int umi_design_system_ready(const UmiDesignSystem *system);
/* Advance the aggregate revision after a governed design-system mutation. */
void umi_design_system_touch(UmiDesignSystem *system);

#ifdef __cplusplus
}
#endif

#endif
