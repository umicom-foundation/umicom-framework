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
UmiStatus umi_enterprise_risk_service_init(UmiEnterpriseRiskService *s,int64_t freshness,int64_t asof){UmiStatus st;if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof *s);st=umi_enterprise_valuation_service_init(&s->valuation,freshness);if(st!=UMI_STATUS_OK)return st;return umi_enterprise_risk_snapshot_init(&s->risk,asof);}
int umi_enterprise_risk_service_ready(const UmiEnterpriseRiskService *s){if(s==NULL)return 0;return(umi_enterprise_valuation_service_ready(&s->valuation)&&s->risk.count>0U)?1:0;}
