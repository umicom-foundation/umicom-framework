/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/requirement.h
 *
 * PURPOSE:
 *   Describe one required or optional native tool for an operation.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable capability. Applications remain thin clients
 *   and must not duplicate discovery, repository policy or operational state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef INCLUDE_UMICOM_TOOLCHAIN_REQUIREMENT_H
#define INCLUDE_UMICOM_TOOLCHAIN_REQUIREMENT_H
#include "umicom/base/status.h"
#include "umicom/base/value_archive.h"
#include "umicom/toolchain/tool.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the toolchain requirement data shared with callers of this public contract.
 */
typedef struct UmiToolchainRequirement {
    UmiToolKind kind;
    int required;
    int validate_version;
} UmiToolchainRequirement;

/**
 * Initialise toolchain requirement from caller-provided values so later operations receive
 * a known state.
 */
void umi_toolchain_requirement_init(UmiToolchainRequirement *requirement,
                                    UmiToolKind kind,
                                    int required);
/**
 * Check that toolchain requirement satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_toolchain_requirement_validate(const UmiToolchainRequirement *requirement);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_toolchain_requirement_archive_encode(const UmiToolchainRequirement *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_toolchain_requirement_archive_decode(const void *bytes, size_t byte_count,
    UmiToolchainRequirement *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_toolchain_requirement_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiToolchainRequirement *expected, UmiToolchainRequirement *value);


#ifdef __cplusplus
}
#endif
#endif
