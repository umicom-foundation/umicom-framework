/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/production/portfolio_report.c
 *
 * PURPOSE:
 *   Implement one bounded part of the Framework-owned application production
 *   control plane while product and frontend code remain independently owned.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/production/portfolio_report.h"

#include <string.h>

UmiStatus umi_application_production_portfolio_report_build(
    const UmiApplicationProductionPortfolio *portfolio,
    UmiApplicationProductionPortfolioReport *out_report)
{
    size_t index;
    unsigned readiness_total = 0U;
    if (portfolio == NULL || out_report == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_report, 0, sizeof(*out_report));
    out_report->application_count = portfolio->count;
    for (index = 0U; index < portfolio->count; ++index) {
        const UmiApplicationProductionRuntime *runtime =
            &portfolio->entries[index].runtime;
        if (!runtime->initialised) return UMI_STATUS_INVALID_STATE;
        if (runtime->acceptance.state == UMI_APPLICATION_PRODUCTION_READY)
            out_report->ready_count += 1U;
        else if (runtime->acceptance.state ==
                 UMI_APPLICATION_PRODUCTION_DEGRADED)
            out_report->degraded_count += 1U;
        else
            out_report->blocked_count += 1U;
        out_report->panel_count += runtime->panels.count;
        out_report->layout_count += runtime->layouts.count;
        out_report->feature_count += runtime->features.count;
        out_report->application_work_count +=
            runtime->features.application_work_count;
        out_report->external_adapter_work_count +=
            runtime->features.external_adapter_work_count;
        readiness_total += runtime->readiness.combined_percent;
    }
    out_report->readiness_percent = portfolio->count == 0U ? 0U :
        readiness_total / (unsigned)portfolio->count;
    return UMI_STATUS_OK;
}

