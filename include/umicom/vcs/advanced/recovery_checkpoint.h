/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/recovery_checkpoint.h
 *
 * PURPOSE:
 *   Capture reversible repository identity before risky VCS operations.
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
#ifndef UMICOM_VCS_ADVANCED_RECOVERY_CHECKPOINT_H
#define UMICOM_VCS_ADVANCED_RECOVERY_CHECKPOINT_H

#include "umicom/vcs/advanced/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the vcs advanced recovery checkpoint data shared with callers of this public
 * contract.
 */
typedef struct UmiVcsAdvancedRecoveryCheckpoint {
    uint32_t struct_size;
    uint32_t api_version;
    char checkpoint_id[UMI_VCS_ADVANCED_ID_CAPACITY];
    char head_oid[UMI_VCS_ADVANCED_OID_CAPACITY];
    char branch[UMI_VCS_ADVANCED_LABEL_CAPACITY];
    char operation[UMI_VCS_ADVANCED_LABEL_CAPACITY];
    uint64_t created_seconds;
    uint64_t worktree_fingerprint;
    int index_dirty;
    int worktree_dirty;
} UmiVcsAdvancedRecoveryCheckpoint;

/**
 * Initialise vcs advanced recovery checkpoint from caller-provided values so later
 * operations receive a known state.
 */
void umi_vcs_advanced_recovery_checkpoint_init(UmiVcsAdvancedRecoveryCheckpoint *value);
/**
 * Check that vcs advanced recovery checkpoint satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_vcs_advanced_recovery_checkpoint_validate(const UmiVcsAdvancedRecoveryCheckpoint *value);
/**
 * Copy vcs advanced recovery checkpoint into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_vcs_advanced_recovery_checkpoint_set(UmiVcsAdvancedRecoveryCheckpoint *value,
                                                     const char *checkpoint_id,
                                                     const char *head_oid,
                                                     const char *branch,
                                                     const char *operation);

#ifdef __cplusplus
}
#endif

#endif
