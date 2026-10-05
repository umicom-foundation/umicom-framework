/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/event_binding.h
 *
 * PURPOSE:
 *   Route a semantic UI event to a command or state action.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_EVENT_BINDING_H
#define UMICOM_UI_REACTIVE_EVENT_BINDING_H
#include "umicom/ui/reactive/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive event binding data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveEventBinding {
    char source_id[UMI_UI_REACTIVE_ID_CAPACITY];
    char event_name[UMI_UI_REACTIVE_ID_CAPACITY];
    char action_id[UMI_UI_REACTIVE_ID_CAPACITY];
    bool enabled;
} UmiUiReactiveEventBinding;
/**
 * Initialise ui reactive event binding from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_event_binding_init(UmiUiReactiveEventBinding *item);
/**
 * Check that ui reactive event binding satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_event_binding_valid(const UmiUiReactiveEventBinding *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_reactive_event_binding_archive_encode(const UmiUiReactiveEventBinding *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_reactive_event_binding_archive_decode(const void *bytes, size_t byte_count,
    UmiUiReactiveEventBinding *value);

#ifdef __cplusplus
}
#endif
#endif
