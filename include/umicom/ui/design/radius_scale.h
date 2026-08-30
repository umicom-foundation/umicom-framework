/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/radius_scale.h
 *
 * PURPOSE:
 *   Define consistent corner radii shared across panels, cards and controls.
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

#ifndef INCLUDE_UMICOM_UI_DESIGN_RADIUS_SCALE_H
#define INCLUDE_UMICOM_UI_DESIGN_RADIUS_SCALE_H

#include "umicom/ui/design/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESIGN_RADIUS_SCALE_LEVELS 8U
typedef struct UmiDesignRadiusScale { double values[8U]; } UmiDesignRadiusScale;
/* Initialise the canonical scale used as a stable design-system baseline. */
UmiDesignRadiusScale radius_scale_default(void);
/* Resolve one validated scale level by zero-based index. */
UmiStatus radius_scale_value(const UmiDesignRadiusScale *scale, size_t index, double *out_value);

#ifdef __cplusplus
}
#endif

#endif
