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

void umi_fc_conformance_report_init(UmiFcConformanceReport *report){if(report!=NULL)*report=(UmiFcConformanceReport){0};}
UmiStatus umi_fc_conformance_report_add(UmiFcConformanceReport *report,const UmiFcConformanceResult *result){if(report==NULL||result==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(report->count>=UMI_FC_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;report->items[report->count++]=*result;if(result->outcome==UMI_FC_PASS)report->passed++;else if(result->outcome==UMI_FC_DEGRADED)report->degraded++;else report->failed++;return UMI_STATUS_OK;}
double umi_fc_conformance_report_score(const UmiFcConformanceReport *report){size_t i;double total=0.0;if(report==NULL||report->count==0U)return 1.0;for(i=0U;i<report->count;++i)total+=report->items[i].score;return umi_fc_clamp_score(total/(double)report->count);}
