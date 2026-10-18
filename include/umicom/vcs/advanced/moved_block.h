/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/moved_block.h
 *
 * PURPOSE:
 *   Capture identical or near-identical blocks moved within a document.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VCS_ADVANCED_MOVED_BLOCK_H
#define UMICOM_VCS_ADVANCED_MOVED_BLOCK_H

#include "umicom/vcs/advanced/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the vcs advanced moved block data shared with callers of this public contract.
 */
typedef struct UmiVcsAdvancedMovedBlock {
    uint32_t struct_size;
    uint32_t api_version;
    size_t old_start;
    size_t new_start;
    size_t line_count;
    uint64_t fingerprint;
    uint32_t confidence_percent;
} UmiVcsAdvancedMovedBlock;

/**
 * Initialise vcs advanced moved block from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_moved_block_init(UmiVcsAdvancedMovedBlock *value);
/**
 * Check that vcs advanced moved block satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_vcs_advanced_moved_block_validate(const UmiVcsAdvancedMovedBlock *value);
/**
 * Provide the vcs advanced moved block is significant operation used by this module and
 * its client applications.
 */
int umi_vcs_advanced_moved_block_is_significant(const UmiVcsAdvancedMovedBlock *value,
                                                   size_t minimum_lines,
                                                   uint32_t minimum_confidence);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_vcs_advanced_moved_block_archive_encode(const UmiVcsAdvancedMovedBlock *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_vcs_advanced_moved_block_archive_decode(const void *bytes, size_t byte_count,
    UmiVcsAdvancedMovedBlock *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiVcsAdvancedMovedBlock object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_vcs_advanced_moved_block_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiVcsAdvancedMovedBlock *workspace, UmiVcsAdvancedMovedBlock *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_vcs_advanced_moved_block_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiVcsAdvancedMovedBlock *expected, UmiVcsAdvancedMovedBlock *value);


#ifdef __cplusplus
}
#endif

#endif
