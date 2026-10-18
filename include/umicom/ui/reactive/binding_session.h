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

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiUiReactiveBindingSession object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_ui_reactive_binding_session_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiUiReactiveBindingSession *workspace, UmiUiReactiveBindingSession *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_ui_reactive_binding_session_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiUiReactiveBindingSession *expected, UmiUiReactiveBindingSession *value);


#ifdef __cplusplus
}
#endif
#endif
