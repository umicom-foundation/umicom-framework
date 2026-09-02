/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/enterprise_risk_service.c
 *
 * PURPOSE:
 *   Aggregate enterprise market-data, valuation and risk state into one Framework service boundary.
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

#include "umicom/finance/enterprise/enterprise_risk_service.h"

#include <string.h>
/*
 * Initialise enterprise risk service from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_enterprise_risk_service_init(UmiEnterpriseRiskService *s,int64_t freshness,int64_t asof){UmiStatus st;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof *s);st=umi_enterprise_valuation_service_init(&s->valuation,freshness);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st;return umi_enterprise_risk_snapshot_init(&s->risk,asof);}
/*
 * Provide the enterprise risk service ready operation used by this module and its client
 * applications.
 */
int umi_enterprise_risk_service_ready(const UmiEnterpriseRiskService *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return 0;return(umi_enterprise_valuation_service_ready(&s->valuation)&&s->risk.count>0U)?1:0;}
