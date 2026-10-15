/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/focus_contract.h
 *
 * PURPOSE:
 *   focusable-element ordering and focus-trap requirements for interactive surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_FOCUS_CONTRACT_H
#define UMICOM_FRONTEND_CONFORMANCE_FOCUS_CONTRACT_H

#include <stddef.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fc focus contract data shared with callers of this public contract.
 */
typedef struct UmiFcFocusContract { size_t focusable_count; size_t traversal_count; bool modal_trap_required; } UmiFcFocusContract;
/**
 * Check that fc focus contract satisfies its contract before another service relies on it.
 */
bool umi_fc_focus_contract_validate(const UmiFcFocusContract *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_fc_focus_contract_archive_encode(const UmiFcFocusContract *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_fc_focus_contract_archive_decode(const void *bytes, size_t byte_count,
    UmiFcFocusContract *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_fc_focus_contract_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiFcFocusContract *expected, UmiFcFocusContract *value);


#ifdef __cplusplus
}
#endif
#endif
