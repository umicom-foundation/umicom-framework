/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/drag_drop_contract.h
 *
 * PURPOSE:
 *   semantic drag/drop operation, keyboard alternative and docking affordance requirements.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_DRAG_DROP_CONTRACT_H
#define UMICOM_FRONTEND_CONFORMANCE_DRAG_DROP_CONTRACT_H

#include <stddef.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fc drag drop contract data shared with callers of this public contract.
 */
typedef struct UmiFcDragDropContract { uint64_t required_ops; bool keyboard_alternative; bool visual_preview; } UmiFcDragDropContract;
/**
 * Check that fc drag drop contract satisfies its contract before another service relies on
 * it.
 */
bool umi_fc_drag_drop_contract_validate(const UmiFcDragDropContract *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_fc_drag_drop_contract_archive_encode(const UmiFcDragDropContract *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_fc_drag_drop_contract_archive_decode(const void *bytes, size_t byte_count,
    UmiFcDragDropContract *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiFcDragDropContract object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_fc_drag_drop_contract_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiFcDragDropContract *workspace, UmiFcDragDropContract *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_fc_drag_drop_contract_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiFcDragDropContract *expected, UmiFcDragDropContract *value);


#ifdef __cplusplus
}
#endif
#endif
