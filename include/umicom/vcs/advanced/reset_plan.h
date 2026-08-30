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
typedef enum UmiVcsAdvancedResetMode { UMI_VCS_RESET_SOFT=0, UMI_VCS_RESET_MIXED=1, UMI_VCS_RESET_HARD=2 } UmiVcsAdvancedResetMode;
typedef struct UmiVcsAdvancedResetPlan {
    uint32_t struct_size; uint32_t api_version; UmiVcsAdvancedResetMode mode;
    char target[UMI_VCS_ADVANCED_OID_CAPACITY]; char path[UMI_VCS_ADVANCED_PATH_CAPACITY];
    int path_limited; UmiVcsSafetyLevel safety; int require_checkpoint;
} UmiVcsAdvancedResetPlan;
void umi_vcs_advanced_reset_plan_init(UmiVcsAdvancedResetPlan *plan);
UmiStatus umi_vcs_advanced_reset_plan_set(UmiVcsAdvancedResetPlan *plan,UmiVcsAdvancedResetMode mode,const char *target);
int umi_vcs_advanced_reset_plan_destructive(const UmiVcsAdvancedResetPlan *plan);
#ifdef __cplusplus
}
#endif
#endif
