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

#ifdef __cplusplus
}
#endif
#endif
