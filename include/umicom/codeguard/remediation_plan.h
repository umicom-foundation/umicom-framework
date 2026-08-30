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
typedef struct UmiCodeGuardRemediationAction {
    char evidence_id[UMI_CODEGUARD_QUALITY_ID_CAPACITY];
    UmiCodeGuardEvidenceState priority;
    char path[UMI_CODEGUARD_QUALITY_PATH_CAPACITY];
    size_t line;
    char instruction[UMI_CODEGUARD_QUALITY_TEXT_CAPACITY];
} UmiCodeGuardRemediationAction;
typedef struct UmiCodeGuardRemediationPlan {
    UmiCodeGuardRemediationAction actions[UMI_CODEGUARD_QUALITY_MAX_ACTIONS];
    size_t count;
} UmiCodeGuardRemediationPlan;
UmiStatus umi_codeguard_remediation_plan_build(const UmiCodeGuardEvidenceStore *evidence,UmiCodeGuardRemediationPlan *out_plan);
const UmiCodeGuardRemediationAction *umi_codeguard_remediation_plan_at(const UmiCodeGuardRemediationPlan *plan,size_t index);
#endif
