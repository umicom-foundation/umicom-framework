/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/remediation/remediation_safety.c
 *
 * PURPOSE:
 *   Implement report whether a generated plan contains any destructive-class proposal.
 *
 * ARCHITECTURE:
 *   Extend the existing Framework-owned repository control and maintenance
 *   capabilities. This module does not duplicate VCS, source-control, lock,
 *   maintenance or doctor state already present in Umicom Framework.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/repository/remediation_safety.h"
/* Report whether a generated plan contains any destructive-class proposal. */
int umi_repository_remediation_plan_safe(const UmiRepositoryRemediationPlan *plan)
{
    return plan != NULL && plan->destructive_count == 0U;
}
