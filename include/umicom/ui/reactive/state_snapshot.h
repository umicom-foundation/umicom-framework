/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/state_snapshot.h
 *
 * PURPOSE:
 *   Represent a named immutable state snapshot reference.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_STATE_SNAPSHOT_H
#define UMICOM_UI_REACTIVE_STATE_SNAPSHOT_H
#include "umicom/ui/reactive/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive state snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveStateSnapshot {
    char snapshot_id[UMI_UI_REACTIVE_ID_CAPACITY];
    uint64_t revision;
    size_t property_count;
    uint64_t fingerprint;
} UmiUiReactiveStateSnapshot;
/**
 * Initialise ui reactive state snapshot from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_state_snapshot_init(UmiUiReactiveStateSnapshot *item);
/**
 * Check that ui reactive state snapshot satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_state_snapshot_valid(const UmiUiReactiveStateSnapshot *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_reactive_state_snapshot_archive_encode(const UmiUiReactiveStateSnapshot *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_reactive_state_snapshot_archive_decode(const void *bytes, size_t byte_count,
    UmiUiReactiveStateSnapshot *value);

#ifdef __cplusplus
}
#endif
#endif
