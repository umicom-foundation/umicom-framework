/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/change_queue.h
 *
 * PURPOSE:
 *   Queue property-change sequence numbers for deterministic dispatch.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_CHANGE_QUEUE_H
#define UMICOM_UI_REACTIVE_CHANGE_QUEUE_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive change queue data shared with callers of this public contract.
 */
typedef struct UmiUiReactiveChangeQueue {
    uint64_t sequences[UMI_UI_REACTIVE_MAX_ITEMS];
    size_t head;
    size_t count;
} UmiUiReactiveChangeQueue;
/**
 * Initialise ui reactive change queue from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_change_queue_init(UmiUiReactiveChangeQueue *item);
/**
 * Provide the ui reactive change queue push operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_reactive_change_queue_push(UmiUiReactiveChangeQueue *item, uint64_t sequence);
/**
 * Provide the ui reactive change queue pop operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_reactive_change_queue_pop(UmiUiReactiveChangeQueue *item, uint64_t *out_sequence);
#ifdef __cplusplus
}
#endif
#endif
