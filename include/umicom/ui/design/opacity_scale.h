/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/opacity_scale.h
 *
 * PURPOSE:
 *   Define common opacity levels for disabled, muted and overlay presentation.
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

#ifndef INCLUDE_UMICOM_UI_DESIGN_OPACITY_SCALE_H
#define INCLUDE_UMICOM_UI_DESIGN_OPACITY_SCALE_H

#include "umicom/ui/design/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESIGN_OPACITY_SCALE_LEVELS 8U
/**
 * Represent the design opacity scale data shared with callers of this public contract.
 */
typedef struct UmiDesignOpacityScale { double values[8U]; } UmiDesignOpacityScale;
/* Initialise the canonical scale used as a stable design-system baseline. */
UmiDesignOpacityScale opacity_scale_default(void);
/* Resolve one validated scale level by zero-based index. */
UmiStatus opacity_scale_value(const UmiDesignOpacityScale *scale, size_t index, double *out_value);

#ifdef __cplusplus
}
#endif

#endif
