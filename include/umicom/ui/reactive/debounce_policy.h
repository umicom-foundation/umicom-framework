/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/debounce_policy.h
 *
 * PURPOSE:
 *   Provide deterministic deadline calculation for debounced updates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_DEBOUNCE_POLICY_H
#define UMICOM_UI_REACTIVE_DEBOUNCE_POLICY_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiReactiveDebouncePolicy {
    uint64_t delay_millis;
    uint64_t last_event_millis;
    uint64_t due_millis;
} UmiUiReactiveDebouncePolicy;
uint64_t umi_ui_reactive_debounce_policy_record(UmiUiReactiveDebouncePolicy *item,uint64_t now_millis);
int umi_ui_reactive_debounce_policy_due(const UmiUiReactiveDebouncePolicy *item,uint64_t now_millis);
#ifdef __cplusplus
}
#endif
#endif
