/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/quality_audit.c
 *
 * PURPOSE:
 *   Implement the quality audit behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/frontend/quality_audit.h"
#include <stdio.h>
#include <string.h>
/*
 * Provide the frontend quality budget default operation used by this module and its client
 * applications.
 */
UmiFrontendQualityBudget umi_frontend_quality_budget_default(void){UmiFrontendQualityBudget b={0};b.max_transfer_bytes=2U*1024U*1024U;b.max_script_bytes=768U*1024U;b.max_render_us=2500000U;b.max_console_errors=0U;b.min_accessibility_score=90U;return b;}
/*
 * Perform frontend quality audit through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_frontend_quality_audit_run(const UmiFrontendQualityBudget *b,const UmiFrontendQualityMetrics *m,UmiFrontendQualityResult *out){int n;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(b==NULL||m==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(out,0,sizeof(*out));out->metrics=*m;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m->transfer_bytes>b->max_transfer_bytes)out->failed_checks+=1U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m->script_bytes>b->max_script_bytes)out->failed_checks+=1U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m->render_us>b->max_render_us)out->failed_checks+=1U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m->console_errors>b->max_console_errors)out->failed_checks+=1U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m->accessibility_score<b->min_accessibility_score)out->failed_checks+=1U;out->passed=out->failed_checks==0U;n=snprintf(out->summary,sizeof(out->summary),"Frontend quality audit: %s (%u failed check%s)",out->passed?"passed":"failed",out->failed_checks,out->failed_checks==1U?"":"s");return n<0||(size_t)n>=sizeof(out->summary)?UMI_STATUS_CAPACITY_EXCEEDED:UMI_STATUS_OK;}
