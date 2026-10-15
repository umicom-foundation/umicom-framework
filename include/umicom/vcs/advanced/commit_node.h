/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/commit_node.h
 *
 * PURPOSE:
 *   Describe one commit in the Framework-owned history graph.
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
#ifndef UMICOM_VCS_ADVANCED_COMMIT_NODE_H
#define UMICOM_VCS_ADVANCED_COMMIT_NODE_H

#include "umicom/vcs/advanced/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the vcs advanced commit node data shared with callers of this public contract.
 */
typedef struct UmiVcsAdvancedCommitNode {
    uint32_t struct_size;
    uint32_t api_version;
    char oid[UMI_VCS_ADVANCED_OID_CAPACITY];
    char subject[UMI_VCS_ADVANCED_TEXT_CAPACITY];
    char author[UMI_VCS_ADVANCED_LABEL_CAPACITY];
    uint64_t timestamp_seconds;
    uint32_t parent_count;
    uint32_t generation;
    int merge_commit;
} UmiVcsAdvancedCommitNode;

/**
 * Initialise vcs advanced commit node from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_commit_node_init(UmiVcsAdvancedCommitNode *value);
/**
 * Check that vcs advanced commit node satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_vcs_advanced_commit_node_validate(const UmiVcsAdvancedCommitNode *value);
/**
 * Copy vcs advanced commit node into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_vcs_advanced_commit_node_set(UmiVcsAdvancedCommitNode *value,
                                              const char *oid,
                                              const char *subject,
                                              const char *author);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_vcs_advanced_commit_node_archive_encode(const UmiVcsAdvancedCommitNode *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_vcs_advanced_commit_node_archive_decode(const void *bytes, size_t byte_count,
    UmiVcsAdvancedCommitNode *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_vcs_advanced_commit_node_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiVcsAdvancedCommitNode *expected, UmiVcsAdvancedCommitNode *value);


#ifdef __cplusplus
}
#endif

#endif
