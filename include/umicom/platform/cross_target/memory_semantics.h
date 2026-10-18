/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/memory_semantics.h
 *
 * PURPOSE:
 *   Describe virtual memory, guard page and executable mapping capabilities without embedding OS APIs in applications.
 *
 * ARCHITECTURE:
 *   Framework owns reusable cross-target and Umicom OS semantics. Existing
 *   compiler/toolchain discovery, platform services and application runtimes
 *   remain authoritative and are composed rather than duplicated here.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLATFORM_CROSS_TARGET_MEMORY_SEMANTICS_H
#define UMICOM_PLATFORM_CROSS_TARGET_MEMORY_SEMANTICS_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ct memory semantics data shared with callers of this public contract.
 */
typedef struct UmiCtMemorySemantics { uint32_t page_size; uint32_t allocation_granularity; bool virtual_memory; bool guard_pages; bool executable_memory; bool huge_pages; } UmiCtMemorySemantics;
/**
 * Check that ct memory semantics satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_ct_memory_semantics_validate(const UmiCtMemorySemantics *semantics);
/**
 * Provide the ct memory round up operation used by this module and its client
 * applications.
 */
uint64_t umi_ct_memory_round_up(const UmiCtMemorySemantics *semantics,uint64_t bytes);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ct_memory_semantics_archive_encode(const UmiCtMemorySemantics *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ct_memory_semantics_archive_decode(const void *bytes, size_t byte_count,
    UmiCtMemorySemantics *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiCtMemorySemantics object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_ct_memory_semantics_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiCtMemorySemantics *workspace, UmiCtMemorySemantics *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_ct_memory_semantics_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiCtMemorySemantics *expected, UmiCtMemorySemantics *value);


#ifdef __cplusplus
}
#endif

#endif
