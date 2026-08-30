/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/remediation/remediation_report.c
 *
 * PURPOSE:
 *   Implement summarise a plan without applying it.
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

#include "umicom/repository/remediation_report.h"
#include <string.h>
#include "umicom/repository/remediation_guard.h"
/* Count automatic eligibility while still reporting destructive proposals as review-only. */
UmiStatus umi_repository_remediation_report_build(const UmiRepositoryRemediationPlan *plan,const UmiRepositoryRemediationPolicy *policy,UmiRepositoryRemediationReport *out_report){size_t i;if(!plan||!policy||!out_report)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(out_report,0,sizeof(*out_report));out_report->action_count=plan->actions.count;for(i=0U;i<plan->actions.count;++i){const UmiRepositoryRemediationAction *a=&plan->actions.items[i];if(umi_repository_remediation_guard_allows(policy,a))++out_report->automatic_eligible;else ++out_report->manual_review;if(a->risk==UMI_REPOSITORY_REMEDIATION_DESTRUCTIVE)out_report->contains_destructive=1;}return UMI_STATUS_OK;}
