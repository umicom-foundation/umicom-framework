/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/remediation/remediation_guard.c
 *
 * PURPOSE:
 *   Implement enforce non-destructive repository planning and explicit review boundaries.
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

#include "umicom/repository/remediation_guard.h"
/* Reject all destructive actions and require explicit policy for review-class actions. */
int umi_repository_remediation_guard_allows(const UmiRepositoryRemediationPolicy *policy,const UmiRepositoryRemediationAction *action){/* Apply this branch only when its contract condition is satisfied. */ if(!policy||!action)return 0;/* Apply this branch only when its contract condition is satisfied. */ if(action->risk==UMI_REPOSITORY_REMEDIATION_DESTRUCTIVE)return 0;/* Apply this branch only when its contract condition is satisfied. */ if(action->risk==UMI_REPOSITORY_REMEDIATION_REVIEW)return policy->allow_review_automatic!=0;return policy->allow_safe_automatic!=0;}
