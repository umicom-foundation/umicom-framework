/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/pan.h
 *
 * PURPOSE:
 *   Provide deterministic canvas panning state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_PAN_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_PAN_H
#include "umicom/designer/visual_designer/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer pan state data shared with callers of this public contract.
 */
typedef struct UmiRadPanState {
    UmiRadPoint offset;
    bool active;
} UmiRadPanState;
/**
 * Initialise visual designer pan from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_rad_pan_init(UmiRadPanState *item);
/**
 * Check that visual designer pan satisfies its contract before another service relies on it.
 */
int umi_rad_pan_is_valid(const UmiRadPanState *item);
#ifdef __cplusplus
}
#endif
#endif
