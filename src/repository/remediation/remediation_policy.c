/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/remediation/remediation_policy.c
 *
 * PURPOSE:
 *   Implement define what repository repairs may be suggested or automatically executed.
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

#include "umicom/repository/remediation_policy.h"
#include <string.h>
/* Enable only explicitly safe automatic actions; this batch itself remains plan-only. */
void umi_repository_remediation_policy_init(UmiRepositoryRemediationPolicy *policy){/* Apply this branch only when its contract condition is satisfied. */ if(!policy)return;(void)memset(policy,0,sizeof(*policy));policy->allow_safe_automatic=1;}
