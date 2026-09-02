/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/remediation/remediation_plan.c
 *
 * PURPOSE:
 *   Implement represent the ordered plan and summary risk counts.
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

#include "umicom/repository/remediation_plan.h"
#include <string.h>
/* Initialise an empty, inspectable plan. */
void umi_repository_remediation_plan_init(UmiRepositoryRemediationPlan *plan){/* Apply this branch only when its contract condition is satisfied. */ if(!plan)return;(void)memset(plan,0,sizeof(*plan));umi_repository_remediation_action_set_init(&plan->actions);}
/* Append one action and maintain aggregate risk counts. */
UmiStatus umi_repository_remediation_plan_add(UmiRepositoryRemediationPlan *plan,const UmiRepositoryRemediationAction *action){UmiStatus s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!plan||!action)return UMI_STATUS_INVALID_ARGUMENT;s=umi_repository_remediation_action_set_add(&plan->actions,action);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(action->risk==UMI_REPOSITORY_REMEDIATION_SAFE)++plan->safe_count;else /* Preserve the original failure result so the caller can respond to the correct cause. */ if(action->risk==UMI_REPOSITORY_REMEDIATION_REVIEW)++plan->review_count;/* Use this fallback path when the earlier condition does not apply. */ else ++plan->destructive_count;++plan->revision;return UMI_STATUS_OK;}
