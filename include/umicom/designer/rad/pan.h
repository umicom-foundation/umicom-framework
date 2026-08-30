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
typedef struct UmiRadPanState {
    UmiRadPoint offset;
    bool active;
} UmiRadPanState;
UmiStatus umi_rad_pan_init(UmiRadPanState *item);
int umi_rad_pan_is_valid(const UmiRadPanState *item);
#ifdef __cplusplus
}
#endif
#endif
