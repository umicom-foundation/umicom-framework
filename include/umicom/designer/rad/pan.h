/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/pan.h
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
#ifndef UMICOM_DESIGNER_RAD_PAN_H
#define UMICOM_DESIGNER_RAD_PAN_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the rad pan state data shared with callers of this public contract.
 */
typedef struct UmiRadPanState {
    UmiRadPoint offset;
    bool active;
} UmiRadPanState;
/**
 * Initialise rad pan from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_rad_pan_init(UmiRadPanState *item);
/**
 * Check that rad pan satisfies its contract before another service relies on it.
 */
int umi_rad_pan_is_valid(const UmiRadPanState *item);
#ifdef __cplusplus
}
#endif
#endif
