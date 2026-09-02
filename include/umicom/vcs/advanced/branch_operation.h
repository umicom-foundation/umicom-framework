/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/branch_operation.h
 *
 * PURPOSE:
 *   Plan branch create, checkout, rename and deletion without executing Git in UI code.
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
#ifndef UMICOM_VCS_ADVANCED_BRANCH_OPERATION_H
#define UMICOM_VCS_ADVANCED_BRANCH_OPERATION_H
#include "umicom/vcs/advanced/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * List the named vcs advanced branch action values accepted by this public contract.
 */
typedef enum UmiVcsAdvancedBranchAction {
    UMI_VCS_BRANCH_CREATE = 0,
    UMI_VCS_BRANCH_CHECKOUT = 1,
    UMI_VCS_BRANCH_RENAME = 2,
    UMI_VCS_BRANCH_DELETE = 3
} UmiVcsAdvancedBranchAction;
/**
 * Represent the vcs advanced branch operation data shared with callers of this public
 * contract.
 */
typedef struct UmiVcsAdvancedBranchOperation {
    uint32_t struct_size;
    uint32_t api_version;
    UmiVcsAdvancedBranchAction action;
    char name[UMI_VCS_ADVANCED_LABEL_CAPACITY];
    char new_name[UMI_VCS_ADVANCED_LABEL_CAPACITY];
    char start_point[UMI_VCS_ADVANCED_OID_CAPACITY];
    int checkout_after_create;
    int force;
    UmiVcsSafetyLevel safety;
} UmiVcsAdvancedBranchOperation;
/**
 * Initialise vcs advanced branch operation from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_branch_operation_init(UmiVcsAdvancedBranchOperation *operation);
/**
 * Check that vcs advanced branch operation satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_vcs_advanced_branch_operation_validate(const UmiVcsAdvancedBranchOperation *operation);
/**
 * Initialise vcs advanced branch operation from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_vcs_advanced_branch_operation_create(UmiVcsAdvancedBranchOperation *operation,
                                                    const char *name,
                                                    const char *start_point,
                                                    int checkout);
/**
 * Provide the vcs advanced branch operation rename operation used by this module and its
 * client applications.
 */
UmiStatus umi_vcs_advanced_branch_operation_rename(UmiVcsAdvancedBranchOperation *operation,
                                                    const char *name,
                                                    const char *new_name);
#ifdef __cplusplus
}
#endif
#endif
