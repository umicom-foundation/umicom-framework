/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/change_queue.h
 *
 * PURPOSE:
 *   Queue property-change sequence numbers for deterministic dispatch.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_CHANGE_QUEUE_H
#define UMICOM_UI_REACTIVE_CHANGE_QUEUE_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiReactiveChangeQueue {
    uint64_t sequences[UMI_UI_REACTIVE_MAX_ITEMS];
    size_t head;
    size_t count;
} UmiUiReactiveChangeQueue;
void umi_ui_reactive_change_queue_init(UmiUiReactiveChangeQueue *item);
UmiStatus umi_ui_reactive_change_queue_push(UmiUiReactiveChangeQueue *item, uint64_t sequence);
UmiStatus umi_ui_reactive_change_queue_pop(UmiUiReactiveChangeQueue *item, uint64_t *out_sequence);
#ifdef __cplusplus
}
#endif
#endif
