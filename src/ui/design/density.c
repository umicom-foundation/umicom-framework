/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/density.c
 *
 * PURPOSE:
 *   Define semantic UI density profiles shared across touch, desktop and high-density workstations.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/density.h"

const char *umi_design_density_name(UmiDesignDensity value)
{
    switch (value) { case UMI_DESIGN_DENSITY_COMPACT: return "compact"; case UMI_DESIGN_DENSITY_STANDARD: return "standard"; case UMI_DESIGN_DENSITY_COMFORTABLE: return "comfortable"; case UMI_DESIGN_DENSITY_TOUCH: return "touch"; default: return "unknown"; }
}
