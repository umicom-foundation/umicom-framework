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
UmiStatus umi_repository_remediation_report_build(const UmiRepositoryRemediationPlan *plan,const UmiRepositoryRemediationPolicy *policy,UmiRepositoryRemediationReport *out_report){size_t i;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!plan||!policy||!out_report)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(out_report,0,sizeof(*out_report));out_report->action_count=plan->actions.count;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<plan->actions.count;++i){const UmiRepositoryRemediationAction *a=&plan->actions.items[i];/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_repository_remediation_guard_allows(policy,a))++out_report->automatic_eligible;/* Use this fallback path when the earlier condition does not apply. */ else ++out_report->manual_review;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(a->risk==UMI_REPOSITORY_REMEDIATION_DESTRUCTIVE)out_report->contains_destructive=1;}return UMI_STATUS_OK;}
