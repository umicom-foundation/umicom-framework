/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/risk_report.h
 *
 * PURPOSE:
 *   Produce one auditable portfolio risk report from primary enterprise measures.
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

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_RISK_REPORT_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_RISK_REPORT_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEnterpriseRiskReport { char portfolio_id[UMI_ENTERPRISE_ID_CAPACITY]; int64_t as_of_ms; double var; double expected_shortfall; double stress_loss; double pnl; } UmiEnterpriseRiskReport;
/* Initialise a coherent portfolio risk report with non-negative risk losses. */
UmiStatus umi_enterprise_risk_report_init(UmiEnterpriseRiskReport *report,const char *portfolio_id,int64_t as_of_ms,double var,double expected_shortfall,double stress_loss,double pnl);

#ifdef __cplusplus
}
#endif

#endif
