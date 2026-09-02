/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/reset_plan.h
 *
 * PURPOSE:
 *   Model soft/mixed/hard reset intent with destructive-operation classification.
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
#ifndef UMICOM_VCS_ADVANCED_RESET_PLAN_H
#define UMICOM_VCS_ADVANCED_RESET_PLAN_H
#include "umicom/vcs/advanced/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * List the named vcs advanced reset mode values accepted by this public contract.
 */
typedef enum UmiVcsAdvancedResetMode { UMI_VCS_RESET_SOFT=0, UMI_VCS_RESET_MIXED=1, UMI_VCS_RESET_HARD=2 } UmiVcsAdvancedResetMode;
/**
 * Represent the vcs advanced reset plan data shared with callers of this public contract.
 */
typedef struct UmiVcsAdvancedResetPlan {
    uint32_t struct_size; uint32_t api_version; UmiVcsAdvancedResetMode mode;
    char target[UMI_VCS_ADVANCED_OID_CAPACITY]; char path[UMI_VCS_ADVANCED_PATH_CAPACITY];
    int path_limited; UmiVcsSafetyLevel safety; int require_checkpoint;
} UmiVcsAdvancedResetPlan;
/**
 * Initialise vcs advanced reset plan from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_reset_plan_init(UmiVcsAdvancedResetPlan *plan);
/**
 * Copy vcs advanced reset plan into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_vcs_advanced_reset_plan_set(UmiVcsAdvancedResetPlan *plan,UmiVcsAdvancedResetMode mode,const char *target);
/**
 * Provide the vcs advanced reset plan destructive operation used by this module and its
 * client applications.
 */
int umi_vcs_advanced_reset_plan_destructive(const UmiVcsAdvancedResetPlan *plan);
#ifdef __cplusplus
}
#endif
#endif
