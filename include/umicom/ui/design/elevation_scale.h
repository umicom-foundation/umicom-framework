/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/elevation_scale.h
 *
 * PURPOSE:
 *   Define renderer-neutral elevation levels for surfaces and transient overlays.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_UI_DESIGN_ELEVATION_SCALE_H
#define INCLUDE_UMICOM_UI_DESIGN_ELEVATION_SCALE_H

#include "umicom/ui/design/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESIGN_ELEVATION_SCALE_LEVELS 6U
typedef struct UmiDesignElevationScale { double depth[6U]; } UmiDesignElevationScale;
/* Initialise the canonical scale used as a stable design-system baseline. */
UmiDesignElevationScale elevation_scale_default(void);
/* Resolve one validated scale level by zero-based index. */
UmiStatus elevation_scale_value(const UmiDesignElevationScale *scale, size_t index, double *out_value);

#ifdef __cplusplus
}
#endif

#endif
