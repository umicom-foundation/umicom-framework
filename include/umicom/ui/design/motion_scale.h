/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/motion_scale.h
 *
 * PURPOSE:
 *   Define standard animation durations for feedback, transitions and workspace motion.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_UI_DESIGN_MOTION_SCALE_H
#define INCLUDE_UMICOM_UI_DESIGN_MOTION_SCALE_H

#include "umicom/ui/design/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESIGN_MOTION_SCALE_LEVELS 6U
typedef struct UmiDesignMotionScale { double milliseconds[6U]; } UmiDesignMotionScale;
/* Initialise the canonical scale used as a stable design-system baseline. */
UmiDesignMotionScale motion_scale_default(void);
/* Resolve one validated scale level by zero-based index. */
UmiStatus motion_scale_value(const UmiDesignMotionScale *scale, size_t index, double *out_value);

#ifdef __cplusplus
}
#endif

#endif
