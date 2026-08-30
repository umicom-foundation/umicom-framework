/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/spacing_scale.h
 *
 * PURPOSE:
 *   Define consistent spatial rhythm for component padding, gaps and layout regions.
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

#ifndef INCLUDE_UMICOM_UI_DESIGN_SPACING_SCALE_H
#define INCLUDE_UMICOM_UI_DESIGN_SPACING_SCALE_H

#include "umicom/ui/design/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESIGN_SPACING_SCALE_LEVELS 10U
typedef struct UmiDesignSpacingScale { double values[10U]; } UmiDesignSpacingScale;
/* Initialise the canonical scale used as a stable design-system baseline. */
UmiDesignSpacingScale spacing_scale_default(void);
/* Resolve one validated scale level by zero-based index. */
UmiStatus spacing_scale_value(const UmiDesignSpacingScale *scale, size_t index, double *out_value);

#ifdef __cplusplus
}
#endif

#endif
