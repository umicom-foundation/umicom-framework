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
typedef struct UmiUiReactiveChangeEvent {
    char path[UMI_UI_REACTIVE_PATH_CAPACITY];
    UmiUiValue before_value;
    UmiUiValue after_value;
    uint64_t sequence;
} UmiUiReactiveChangeEvent;
void umi_ui_reactive_change_event_init(UmiUiReactiveChangeEvent *item);
int umi_ui_reactive_change_event_valid(const UmiUiReactiveChangeEvent *item);
#ifdef __cplusplus
}
#endif
#endif
