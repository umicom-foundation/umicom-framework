/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/enterprise_risk_service.h
 *
 * PURPOSE:
 *   Aggregate enterprise market-data, valuation and risk state into one Framework service boundary.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_ENTERPRISE_RISK_SERVICE_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_ENTERPRISE_RISK_SERVICE_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/finance/enterprise/valuation_service.h"
#include "umicom/finance/enterprise/risk_snapshot.h"
typedef struct UmiEnterpriseRiskService { UmiEnterpriseValuationService valuation; UmiEnterpriseRiskSnapshot risk; uint64_t scenario_runs; uint64_t report_runs; } UmiEnterpriseRiskService;
/* Initialise the enterprise valuation/risk aggregate at one risk as-of time. */
UmiStatus umi_enterprise_risk_service_init(UmiEnterpriseRiskService *service,int64_t freshness_ms,int64_t risk_as_of_ms);
/* Return one when valuation inputs/workers exist and at least one risk report is available. */
int umi_enterprise_risk_service_ready(const UmiEnterpriseRiskService *service);

#ifdef __cplusplus
}
#endif

#endif
