/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/remediation_plan.h
 *
 * PURPOSE:
 *   Represent the ordered plan and summary risk counts.
 *
 * ARCHITECTURE:
 *   Extend the existing Framework-owned repository control and maintenance
 *   capabilities. This module does not duplicate VCS, source-control, lock,
 *   maintenance or doctor state already present in Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_REPOSITORY_REMEDIATION_PLAN_H
#define INCLUDE_UMICOM_REPOSITORY_REMEDIATION_PLAN_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/repository/remediation_types.h"
#include "umicom/repository/remediation_policy.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "umicom/repository/remediation_action_set.h"
typedef struct UmiRepositoryRemediationPlan { UmiRepositoryRemediationActionSet actions; size_t safe_count; size_t review_count; size_t destructive_count; uint64_t revision; } UmiRepositoryRemediationPlan;
/* Initialise an empty plan. */
void umi_repository_remediation_plan_init(UmiRepositoryRemediationPlan *plan);
/* Append and classify one action. */
UmiStatus umi_repository_remediation_plan_add(UmiRepositoryRemediationPlan *plan,const UmiRepositoryRemediationAction *action);
#ifdef __cplusplus
}
#endif
#endif
