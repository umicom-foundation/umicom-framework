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
#include "umicom/base/value_archive.h"
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
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_reactive_change_event_archive_encode(const UmiUiReactiveChangeEvent *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_reactive_change_event_archive_decode(const void *bytes, size_t byte_count,
    UmiUiReactiveChangeEvent *value);

#ifdef __cplusplus
}
#endif
#endif
