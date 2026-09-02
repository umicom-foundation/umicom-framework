/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/throttle_policy.h
 *
 * PURPOSE:
 *   Provide deterministic admission rules for throttled updates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_THROTTLE_POLICY_H
#define UMICOM_UI_REACTIVE_THROTTLE_POLICY_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive throttle policy data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveThrottlePolicy {
    uint64_t interval_millis;
    uint64_t last_dispatch_millis;
} UmiUiReactiveThrottlePolicy;
/**
 * Provide the ui reactive throttle policy admit operation used by this module and its
 * client applications.
 */
int umi_ui_reactive_throttle_policy_admit(UmiUiReactiveThrottlePolicy *item,uint64_t now_millis);
#ifdef __cplusplus
}
#endif
#endif
