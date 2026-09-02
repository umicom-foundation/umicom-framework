/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/remediation_plan.h
 *
 * PURPOSE:
 *   Publish the public remediation plan contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_CODEGUARD_REMEDIATION_PLAN_H
#define UMICOM_CODEGUARD_REMEDIATION_PLAN_H
#include "umicom/codeguard/evidence.h"
/**
 * Represent the code guard remediation action data shared with callers of this public
 * contract.
 */
typedef struct UmiCodeGuardRemediationAction {
    char evidence_id[UMI_CODEGUARD_QUALITY_ID_CAPACITY];
    UmiCodeGuardEvidenceState priority;
    char path[UMI_CODEGUARD_QUALITY_PATH_CAPACITY];
    size_t line;
    char instruction[UMI_CODEGUARD_QUALITY_TEXT_CAPACITY];
} UmiCodeGuardRemediationAction;
/**
 * Represent the code guard remediation plan data shared with callers of this public
 * contract.
 */
typedef struct UmiCodeGuardRemediationPlan {
    UmiCodeGuardRemediationAction actions[UMI_CODEGUARD_QUALITY_MAX_ACTIONS];
    size_t count;
} UmiCodeGuardRemediationPlan;
/**
 * Provide the codeguard remediation plan build operation used by this module and its
 * client applications.
 */
UmiStatus umi_codeguard_remediation_plan_build(const UmiCodeGuardEvidenceStore *evidence,UmiCodeGuardRemediationPlan *out_plan);
/**
 * Find codeguard remediation plan while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiCodeGuardRemediationAction *umi_codeguard_remediation_plan_at(const UmiCodeGuardRemediationPlan *plan,size_t index);
#endif
