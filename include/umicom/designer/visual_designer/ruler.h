/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/ruler.h
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
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_RULER_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_RULER_H
#include "umicom/designer/visual_designer/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer ruler data shared with callers of this public contract.
 */
typedef struct UmiRadRuler {
    UmiRadOrientation orientation;
    int32_t origin;
    int32_t major_step;
    bool visible;
} UmiRadRuler;
/**
 * Initialise visual designer ruler from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_rad_ruler_init(UmiRadRuler *item);
/**
 * Check that visual designer ruler satisfies its contract before another service relies on it.
 */
int umi_rad_ruler_is_valid(const UmiRadRuler *item);
#ifdef __cplusplus
}
#endif
#endif
