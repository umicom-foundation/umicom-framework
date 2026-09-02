/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/conformance_report.c
 *
 * PURPOSE:
 *   aggregate result collection, counts and score calculation for one frontend run.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/conformance_report.h"

/*
 * Initialise fc conformance report from caller-provided values so later operations receive
 * a known state.
 */
void umi_fc_conformance_report_init(UmiFcConformanceReport *report){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(report!=NULL)*report=(UmiFcConformanceReport){0};}
/*
 * Add fc conformance report only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_fc_conformance_report_add(UmiFcConformanceReport *report,const UmiFcConformanceResult *result){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(report==NULL||result==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(report->count>=UMI_FC_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;report->items[report->count++]=*result;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(result->outcome==UMI_FC_PASS)report->passed++;else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(result->outcome==UMI_FC_DEGRADED)report->degraded++;/* Use this fallback path when the earlier condition does not apply. */ else report->failed++;return UMI_STATUS_OK;}
/*
 * Provide the fc conformance report score operation used by this module and its client
 * applications.
 */
double umi_fc_conformance_report_score(const UmiFcConformanceReport *report){size_t i;double total=0.0;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(report==NULL||report->count==0U)return 1.0;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<report->count;++i)total+=report->items[i].score;return umi_fc_clamp_score(total/(double)report->count);}
