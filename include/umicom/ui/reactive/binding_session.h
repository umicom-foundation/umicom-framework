/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/binding_session.h
 *
 * PURPOSE:
 *   Track binding activation, revision and propagation counts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_BINDING_SESSION_H
#define UMICOM_UI_REACTIVE_BINDING_SESSION_H
#include "umicom/ui/reactive/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive binding session data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveBindingSession {
    char session_id[UMI_UI_REACTIVE_ID_CAPACITY];
    bool active;
    uint64_t revision;
    size_t propagations;
} UmiUiReactiveBindingSession;
/**
 * Initialise ui reactive binding session from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_binding_session_init(UmiUiReactiveBindingSession *item);
/**
 * Check that ui reactive binding session satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_binding_session_valid(const UmiUiReactiveBindingSession *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_reactive_binding_session_archive_encode(const UmiUiReactiveBindingSession *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_reactive_binding_session_archive_decode(const void *bytes, size_t byte_count,
    UmiUiReactiveBindingSession *value);

#ifdef __cplusplus
}
#endif
#endif
