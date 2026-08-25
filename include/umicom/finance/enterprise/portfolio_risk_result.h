/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/portfolio_risk_result.h
 *
 * PURPOSE:
 *   Capture portfolio VaR, expected shortfall and stress loss in one reusable record.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_PORTFOLIO_RISK_RESULT_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_PORTFOLIO_RISK_RESULT_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEnterprisePortfolioRiskResult { char portfolio_id[UMI_ENTERPRISE_ID_CAPACITY]; double value_at_risk; double expected_shortfall; double worst_stress_loss; } UmiEnterprisePortfolioRiskResult;
/* Initialise coherent non-negative portfolio risk measures. */
UmiStatus umi_enterprise_portfolio_risk_result_init(UmiEnterprisePortfolioRiskResult *result,const char *portfolio_id,double value_at_risk,double expected_shortfall,double worst_stress_loss);

#ifdef __cplusplus
}
#endif

#endif
