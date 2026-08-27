/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/production/portfolio_report.h
 *
 * PURPOSE:
 *   Publish one bounded contract in the Framework-owned application production
 *   control plane without moving business logic into the Master Controller.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTION_PORTFOLIO_REPORT_H
#define UMICOM_APPLICATION_PRODUCTION_PORTFOLIO_REPORT_H

#include "umicom/application/production/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/application/production/portfolio.h"

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

UmiStatus umi_application_production_portfolio_report_build(
    const UmiApplicationProductionPortfolio *portfolio,
    UmiApplicationProductionPortfolioReport *out_report);

#ifdef __cplusplus
}
#endif
#endif
