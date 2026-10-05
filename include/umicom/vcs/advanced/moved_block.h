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

#ifdef __cplusplus
}
#endif

#endif
