/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/change_event.h
 *
 * PURPOSE:
 *   Describe one observable property change with monotonic sequence metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_CHANGE_EVENT_H
#define UMICOM_UI_REACTIVE_CHANGE_EVENT_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive change event data shared with callers of this public contract.
 */
typedef struct UmiUiReactiveChangeEvent {
    char path[UMI_UI_REACTIVE_PATH_CAPACITY];
    UmiUiValue before_value;
    UmiUiValue after_value;
    uint64_t sequence;
} UmiUiReactiveChangeEvent;
/**
 * Initialise ui reactive change event from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_change_event_init(UmiUiReactiveChangeEvent *item);
/**
 * Check that ui reactive change event satisfies its contract before another service relies
 * on it.
 */
int umi_ui_reactive_change_event_valid(const UmiUiReactiveChangeEvent *item);
#ifdef __cplusplus
}
#endif
#endif
