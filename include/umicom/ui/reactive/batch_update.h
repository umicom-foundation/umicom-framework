/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/batch_update.h
 *
 * PURPOSE:
 *   Aggregate state mutations into one revision boundary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_BATCH_UPDATE_H
#define UMICOM_UI_REACTIVE_BATCH_UPDATE_H
#include "umicom/ui/reactive/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive batch update data shared with callers of this public contract.
 */
typedef struct UmiUiReactiveBatchUpdate {
    size_t mutation_count;
    uint64_t start_revision;
    uint64_t end_revision;
    bool committed;
} UmiUiReactiveBatchUpdate;
/**
 * Initialise ui reactive batch update from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_batch_update_init(UmiUiReactiveBatchUpdate *item);
/**
 * Check that ui reactive batch update satisfies its contract before another service relies
 * on it.
 */
int umi_ui_reactive_batch_update_valid(const UmiUiReactiveBatchUpdate *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_reactive_batch_update_archive_encode(const UmiUiReactiveBatchUpdate *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_reactive_batch_update_archive_decode(const void *bytes, size_t byte_count,
    UmiUiReactiveBatchUpdate *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_ui_reactive_batch_update_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiUiReactiveBatchUpdate *expected, UmiUiReactiveBatchUpdate *value);


#ifdef __cplusplus
}
#endif
#endif
