/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/application_brand.h
 *
 * PURPOSE:
 *   Represent application-level identity as semantic accents and density defaults without embedding toolkit-specific styling.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_UI_DESIGN_APPLICATION_BRAND_H
#define INCLUDE_UMICOM_UI_DESIGN_APPLICATION_BRAND_H

#include "umicom/ui/design/types.h"
#include "umicom/ui/design/color.h"
#include "umicom/ui/design/density.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/ui/design/color.h"
#include "umicom/ui/design/density.h"
typedef struct UmiDesignApplicationBrand { char application_id[UMI_DESIGN_ID_CAPACITY]; char display_name[UMI_DESIGN_NAME_CAPACITY]; UmiDesignRgba accent; UmiDesignRgba secondary; UmiDesignDensity density; } UmiDesignApplicationBrand;
/* Initialise an application brand profile from semantic colours and density. */
UmiStatus umi_design_application_brand_init(UmiDesignApplicationBrand *brand, const char *application_id, const char *display_name, UmiDesignRgba accent, UmiDesignRgba secondary, UmiDesignDensity density);
/* Load a Framework-owned baseline brand profile for a known Umicom application identifier. */
UmiStatus umi_design_application_brand_preset(const char *application_id, UmiDesignApplicationBrand *out_brand);

#ifdef __cplusplus
}
#endif

#endif
