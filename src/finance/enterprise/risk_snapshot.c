/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/risk_snapshot.c
 *
 * PURPOSE:
 *   Aggregate multiple portfolio risk reports at one enterprise as-of time.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/enterprise/risk_snapshot.h"

#include <string.h>
UmiStatus umi_enterprise_risk_snapshot_init(UmiEnterpriseRiskSnapshot *s,int64_t asof){if(s==NULL||asof<0)return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof *s);s->as_of_ms=asof;return UMI_STATUS_OK;}
UmiStatus umi_enterprise_risk_snapshot_add(UmiEnterpriseRiskSnapshot *s,const UmiEnterpriseRiskReport *r){if(s==NULL||r==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(r->as_of_ms!=s->as_of_ms)return UMI_STATUS_INVALID_STATE;if(s->count>=64U)return UMI_STATUS_CAPACITY_EXCEEDED;s->reports[s->count++]=*r;return UMI_STATUS_OK;}
double umi_enterprise_risk_snapshot_sum_var(const UmiEnterpriseRiskSnapshot *s){size_t i;double total=0.0;if(s==NULL)return 0.0;for(i=0U;i<s->count;++i)total+=s->reports[i].var;return total;}
