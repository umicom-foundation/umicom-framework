/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/loop_prevention.h
 *
 * PURPOSE:
 *   Remember recent propagation fingerprints and reject immediate loops.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_LOOP_PREVENTION_H
#define UMICOM_UI_REACTIVE_LOOP_PREVENTION_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiReactiveLoopPrevention {
    uint64_t fingerprints[64];
    size_t count;
} UmiUiReactiveLoopPrevention;
void umi_ui_reactive_loop_prevention_init(UmiUiReactiveLoopPrevention *item);
UmiStatus umi_ui_reactive_loop_prevention_remember(UmiUiReactiveLoopPrevention *item,uint64_t fingerprint);
#ifdef __cplusplus
}
#endif
#endif
