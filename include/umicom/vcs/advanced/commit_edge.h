/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/commit_edge.h
 *
 * PURPOSE:
 *   Describe a directed parent relationship between history graph commits.
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
#ifndef UMICOM_VCS_ADVANCED_COMMIT_EDGE_H
#define UMICOM_VCS_ADVANCED_COMMIT_EDGE_H

#include "umicom/vcs/advanced/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the vcs advanced commit edge data shared with callers of this public contract.
 */
typedef struct UmiVcsAdvancedCommitEdge {
    uint32_t struct_size;
    uint32_t api_version;
    char child_oid[UMI_VCS_ADVANCED_OID_CAPACITY];
    char parent_oid[UMI_VCS_ADVANCED_OID_CAPACITY];
    uint32_t parent_index;
} UmiVcsAdvancedCommitEdge;

/**
 * Initialise vcs advanced commit edge from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_commit_edge_init(UmiVcsAdvancedCommitEdge *value);
/**
 * Check that vcs advanced commit edge satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_vcs_advanced_commit_edge_validate(const UmiVcsAdvancedCommitEdge *value);
/**
 * Copy vcs advanced commit edge into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_vcs_advanced_commit_edge_set(UmiVcsAdvancedCommitEdge *value,
                                              const char *child_oid,
                                              const char *parent_oid,
                                              uint32_t parent_index);

#ifdef __cplusplus
}
#endif

#endif
