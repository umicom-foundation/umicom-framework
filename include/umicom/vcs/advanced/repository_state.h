/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/repository_state.h
 *
 * PURPOSE:
 *   Aggregate branch/upstream and in-progress Git operation state.
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
#ifndef UMICOM_VCS_ADVANCED_REPOSITORY_STATE_H
#define UMICOM_VCS_ADVANCED_REPOSITORY_STATE_H

#include "umicom/vcs/advanced/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the vcs advanced repository state data shared with callers of this public
 * contract.
 */
typedef struct UmiVcsAdvancedRepositoryState {
    uint32_t struct_size;
    uint32_t api_version;
    char branch[UMI_VCS_ADVANCED_LABEL_CAPACITY];
    char upstream[UMI_VCS_ADVANCED_LABEL_CAPACITY];
    char head_oid[UMI_VCS_ADVANCED_OID_CAPACITY];
    size_t ahead;
    size_t behind;
    size_t conflicts;
    int detached_head;
    int merge_in_progress;
    int rebase_in_progress;
    int cherry_pick_in_progress;
    int revert_in_progress;
    int bisect_in_progress;
} UmiVcsAdvancedRepositoryState;

/**
 * Initialise vcs advanced repository state from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_repository_state_init(UmiVcsAdvancedRepositoryState *value);
/**
 * Check that vcs advanced repository state satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_vcs_advanced_repository_state_validate(const UmiVcsAdvancedRepositoryState *value);
/**
 * Provide the vcs advanced repository state operation in progress operation used by this
 * module and its client applications.
 */
int umi_vcs_advanced_repository_state_operation_in_progress(const UmiVcsAdvancedRepositoryState *value);
/**
 * Provide the vcs advanced repository state diverged operation used by this module and its
 * client applications.
 */
int umi_vcs_advanced_repository_state_diverged(const UmiVcsAdvancedRepositoryState *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_vcs_advanced_repository_state_archive_encode(const UmiVcsAdvancedRepositoryState *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_vcs_advanced_repository_state_archive_decode(const void *bytes, size_t byte_count,
    UmiVcsAdvancedRepositoryState *value);

#ifdef __cplusplus
}
#endif

#endif
