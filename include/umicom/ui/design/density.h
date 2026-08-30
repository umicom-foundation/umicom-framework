/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/density.h
 *
 * PURPOSE:
 *   Define semantic UI density profiles shared across touch, desktop and high-density workstations.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_UI_DESIGN_DENSITY_H
#define INCLUDE_UMICOM_UI_DESIGN_DENSITY_H

#include "umicom/ui/design/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiDesignDensity {
    UMI_DESIGN_DENSITY_COMPACT = 1,
    UMI_DESIGN_DENSITY_STANDARD = 2,
    UMI_DESIGN_DENSITY_COMFORTABLE = 3,
    UMI_DESIGN_DENSITY_TOUCH = 4
} UmiDesignDensity;

/* Return a stable semantic name for the design enumeration value. */
const char *umi_design_density_name(UmiDesignDensity value);

#ifdef __cplusplus
}
#endif

#endif
