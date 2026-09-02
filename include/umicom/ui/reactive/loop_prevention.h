/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/loop_prevention.h
 *
 * PURPOSE:
 *   Remember recent propagation fingerprints and reject immediate loops.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_LOOP_PREVENTION_H
#define UMICOM_UI_REACTIVE_LOOP_PREVENTION_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive loop prevention data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveLoopPrevention {
    uint64_t fingerprints[64];
    size_t count;
} UmiUiReactiveLoopPrevention;
/**
 * Initialise ui reactive loop prevention from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_loop_prevention_init(UmiUiReactiveLoopPrevention *item);
/**
 * Provide the ui reactive loop prevention remember operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_reactive_loop_prevention_remember(UmiUiReactiveLoopPrevention *item,uint64_t fingerprint);
#ifdef __cplusplus
}
#endif
#endif
