/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/state_restore.h
 *
 * PURPOSE:
 *   Represent governed state restoration intent and result.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_STATE_RESTORE_H
#define UMICOM_UI_REACTIVE_STATE_RESTORE_H
#include "umicom/ui/reactive/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive state restore data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveStateRestore {
    char snapshot_id[UMI_UI_REACTIVE_ID_CAPACITY];
    uint64_t from_revision;
    uint64_t to_revision;
    bool completed;
} UmiUiReactiveStateRestore;
/**
 * Initialise ui reactive state restore from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_state_restore_init(UmiUiReactiveStateRestore *item);
/**
 * Check that ui reactive state restore satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_state_restore_valid(const UmiUiReactiveStateRestore *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_reactive_state_restore_archive_encode(const UmiUiReactiveStateRestore *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_reactive_state_restore_archive_decode(const void *bytes, size_t byte_count,
    UmiUiReactiveStateRestore *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_ui_reactive_state_restore_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiUiReactiveStateRestore *expected, UmiUiReactiveStateRestore *value);


#ifdef __cplusplus
}
#endif
#endif
