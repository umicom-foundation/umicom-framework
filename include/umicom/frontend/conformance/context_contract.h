/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/context_contract.h
 *
 * PURPOSE:
 *   typed context-channel requirements for linked cross-application surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_CONTEXT_CONTRACT_H
#define UMICOM_FRONTEND_CONFORMANCE_CONTEXT_CONTRACT_H

#include <stddef.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fc context contract data shared with callers of this public contract.
 */
typedef struct UmiFcContextContract { uint64_t required_types; bool bidirectional; bool accessible_label; } UmiFcContextContract;
/**
 * Check that fc context contract satisfies its contract before another service relies on
 * it.
 */
bool umi_fc_context_contract_validate(const UmiFcContextContract *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_fc_context_contract_archive_encode(const UmiFcContextContract *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_fc_context_contract_archive_decode(const void *bytes, size_t byte_count,
    UmiFcContextContract *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiFcContextContract object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_fc_context_contract_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiFcContextContract *workspace, UmiFcContextContract *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_fc_context_contract_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiFcContextContract *expected, UmiFcContextContract *value);


#ifdef __cplusplus
}
#endif
#endif
