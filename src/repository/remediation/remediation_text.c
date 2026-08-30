/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/remediation/remediation_text.c
 *
 * PURPOSE:
 *   Implement format remediation proposals as an explicit plan rather than hidden automation.
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

#include "umicom/repository/remediation_text.h"
#include <stdio.h>
/* Render each plan action in order so developers can review every proposed step. */
UmiStatus umi_repository_remediation_text_format(const UmiRepositoryRemediationPlan *plan,char *out_text,size_t capacity){size_t i,used=0U;if(!plan||!out_text||capacity==0U)return UMI_STATUS_INVALID_ARGUMENT;out_text[0]='\0';for(i=0U;i<plan->actions.count;++i){const UmiRepositoryRemediationAction *a=&plan->actions.items[i];int n=snprintf(out_text+used,capacity-used,"[%s] %s\n  %s\n  Preview: %s\n",umi_repository_remediation_risk_text(a->risk),a->title,a->rationale,a->command_preview);if(n<0)return UMI_STATUS_IO_ERROR;if((size_t)n>=capacity-used)return UMI_STATUS_CAPACITY_EXCEEDED;used+=(size_t)n;}return UMI_STATUS_OK;}
