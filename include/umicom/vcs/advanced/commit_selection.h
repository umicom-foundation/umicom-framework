/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/commit_selection.h
 *
 * PURPOSE:
 *   Maintain ordered, de-duplicated commit selections for history operations.
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
#ifndef UMICOM_VCS_ADVANCED_COMMIT_SELECTION_H
#define UMICOM_VCS_ADVANCED_COMMIT_SELECTION_H
#include "umicom/vcs/advanced/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the vcs advanced commit selection data shared with callers of this public
 * contract.
 */
typedef struct UmiVcsAdvancedCommitSelection {
    uint32_t struct_size; uint32_t api_version;
    char oids[UMI_VCS_ADVANCED_SMALL_CAPACITY][UMI_VCS_ADVANCED_OID_CAPACITY]; size_t count;
} UmiVcsAdvancedCommitSelection;
/**
 * Initialise vcs advanced commit selection from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_commit_selection_init(UmiVcsAdvancedCommitSelection *selection);
/**
 * Add vcs advanced commit selection only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_vcs_advanced_commit_selection_add(UmiVcsAdvancedCommitSelection *selection,const char *oid);
/**
 * Remove vcs advanced commit selection while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_vcs_advanced_commit_selection_remove(UmiVcsAdvancedCommitSelection *selection,const char *oid);
/**
 * Provide the vcs advanced commit selection contains operation used by this module and its
 * client applications.
 */
int umi_vcs_advanced_commit_selection_contains(const UmiVcsAdvancedCommitSelection *selection,const char *oid);
#ifdef __cplusplus
}
#endif
#endif
