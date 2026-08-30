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

void umi_vcs_advanced_commit_node_init(UmiVcsAdvancedCommitNode *value);
UmiStatus umi_vcs_advanced_commit_node_validate(const UmiVcsAdvancedCommitNode *value);
UmiStatus umi_vcs_advanced_commit_node_set(UmiVcsAdvancedCommitNode *value,
                                              const char *oid,
                                              const char *subject,
                                              const char *author);

#ifdef __cplusplus
}
#endif

#endif
