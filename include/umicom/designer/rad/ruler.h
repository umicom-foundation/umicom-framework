/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/ruler.h
 *
 * PURPOSE:
 *   Describe design-time rulers and origin offsets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_RULER_H
#define UMICOM_DESIGNER_RAD_RULER_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the rad ruler data shared with callers of this public contract.
 */
typedef struct UmiRadRuler {
    UmiRadOrientation orientation;
    int32_t origin;
    int32_t major_step;
    bool visible;
} UmiRadRuler;
/**
 * Initialise rad ruler from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_rad_ruler_init(UmiRadRuler *item);
/**
 * Check that rad ruler satisfies its contract before another service relies on it.
 */
int umi_rad_ruler_is_valid(const UmiRadRuler *item);
#ifdef __cplusplus
}
#endif
#endif
