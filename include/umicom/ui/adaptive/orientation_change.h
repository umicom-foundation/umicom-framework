/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/adaptive/orientation_change.h
 *
 * PURPOSE:
 *   Represent and apply deterministic orientation changes to logical viewport and safe-area state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_ADAPTIVE_ORIENTATION_CHANGE_H
#define UMICOM_UI_ADAPTIVE_ORIENTATION_CHANGE_H
#include "umicom/ui/adaptive/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the adaptive orientation change data shared with callers of this public
 * contract.
 */
typedef struct UmiAdaptiveOrientationChange {
    UmiAdaptiveDisplayOrientation from;
    UmiAdaptiveDisplayOrientation to;
    UmiAdaptiveViewport viewport;
    UmiAdaptiveInsets safe_area;
} UmiAdaptiveOrientationChange;
/* Rotate a viewport and safe-area insets by one quarter turn. */
UmiStatus umi_adaptive_orientation_change_rotate(UmiAdaptiveViewport viewport,
                                                 UmiAdaptiveInsets safe_area,
                                                 int clockwise,
                                                 UmiAdaptiveOrientationChange *out_change);

#ifdef __cplusplus
}
#endif
#endif
