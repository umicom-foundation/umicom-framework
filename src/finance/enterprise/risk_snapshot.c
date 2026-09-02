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
/*
 * Initialise enterprise risk snapshot from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_enterprise_risk_snapshot_init(UmiEnterpriseRiskSnapshot *s,int64_t asof){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||asof<0)return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof *s);s->as_of_ms=asof;return UMI_STATUS_OK;}
/*
 * Add enterprise risk snapshot only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_enterprise_risk_snapshot_add(UmiEnterpriseRiskSnapshot *s,const UmiEnterpriseRiskReport *r){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||r==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r->as_of_ms!=s->as_of_ms)return UMI_STATUS_INVALID_STATE;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->count>=64U)return UMI_STATUS_CAPACITY_EXCEEDED;s->reports[s->count++]=*r;return UMI_STATUS_OK;}
/*
 * Provide the enterprise risk snapshot sum var operation used by this module and its
 * client applications.
 */
double umi_enterprise_risk_snapshot_sum_var(const UmiEnterpriseRiskSnapshot *s){size_t i;double total=0.0;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return 0.0;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<s->count;++i)total+=s->reports[i].var;return total;}
