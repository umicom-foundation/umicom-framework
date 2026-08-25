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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VCS_ADVANCED_COMMIT_EDGE_H
#define UMICOM_VCS_ADVANCED_COMMIT_EDGE_H

#include "umicom/vcs/advanced/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiVcsAdvancedCommitEdge {
    uint32_t struct_size;
    uint32_t api_version;
    char child_oid[UMI_VCS_ADVANCED_OID_CAPACITY];
    char parent_oid[UMI_VCS_ADVANCED_OID_CAPACITY];
    uint32_t parent_index;
} UmiVcsAdvancedCommitEdge;

void umi_vcs_advanced_commit_edge_init(UmiVcsAdvancedCommitEdge *value);
UmiStatus umi_vcs_advanced_commit_edge_validate(const UmiVcsAdvancedCommitEdge *value);
UmiStatus umi_vcs_advanced_commit_edge_set(UmiVcsAdvancedCommitEdge *value,
                                              const char *child_oid,
                                              const char *parent_oid,
                                              uint32_t parent_index);

#ifdef __cplusplus
}
#endif

#endif
