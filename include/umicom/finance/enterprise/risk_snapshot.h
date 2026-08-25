/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/risk_snapshot.h
 *
 * PURPOSE:
 *   Aggregate multiple portfolio risk reports at one enterprise as-of time.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_RISK_SNAPSHOT_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_RISK_SNAPSHOT_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/finance/enterprise/risk_report.h"
typedef struct UmiEnterpriseRiskSnapshot { int64_t as_of_ms; UmiEnterpriseRiskReport reports[64U]; size_t count; } UmiEnterpriseRiskSnapshot;
/* Initialise an empty risk snapshot. */
UmiStatus umi_enterprise_risk_snapshot_init(UmiEnterpriseRiskSnapshot *snapshot,int64_t as_of_ms);
/* Add a report only when it belongs to the same as-of cut. */
UmiStatus umi_enterprise_risk_snapshot_add(UmiEnterpriseRiskSnapshot *snapshot,const UmiEnterpriseRiskReport *report);
/* Sum standalone portfolio VaR values for conservative dashboard display. */
double umi_enterprise_risk_snapshot_sum_var(const UmiEnterpriseRiskSnapshot *snapshot);

#ifdef __cplusplus
}
#endif

#endif
