/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/production/portfolio_report.h
 *
 * PURPOSE:
 *   Publish one bounded contract in the Framework-owned application production
 *   control plane without moving business logic into the Master Controller.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTION_PORTFOLIO_REPORT_H
#define UMICOM_APPLICATION_PRODUCTION_PORTFOLIO_REPORT_H

#include "umicom/application/production/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/application/production/portfolio.h"

/**
 * Represent the application production portfolio report data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationProductionPortfolioReport {
    size_t application_count;
    size_t ready_count;
    size_t degraded_count;
    size_t blocked_count;
    size_t panel_count;
    size_t layout_count;
    size_t feature_count;
    size_t application_work_count;
    size_t external_adapter_work_count;
    unsigned readiness_percent;
} UmiApplicationProductionPortfolioReport;

/**
 * Provide the application production portfolio report build operation used by this module
 * and its client applications.
 */
UmiStatus umi_application_production_portfolio_report_build(
    const UmiApplicationProductionPortfolio *portfolio,
    UmiApplicationProductionPortfolioReport *out_report);

#ifdef __cplusplus
}
#endif
#endif
