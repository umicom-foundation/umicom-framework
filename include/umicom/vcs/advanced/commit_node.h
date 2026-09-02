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

#ifdef __cplusplus
}
#endif

#endif
