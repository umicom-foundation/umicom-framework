/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/production/readiness_report.c
 *
 * PURPOSE:
 *   Implement one bounded part of the Framework-owned application production
 *   control plane while product and frontend code remain independently owned.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/production/readiness_report.h"

#include <string.h>

UmiStatus umi_application_production_readiness_report_build(
    const UmiApplicationProductionBinding *binding,
    const UmiApplicationProductionCapabilityReadiness *capabilities,
    UmiApplicationProductionReadinessReport *out_report)
{
    UmiStatus status;
    if (capabilities == NULL || out_report == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_production_binding_validate(binding);
    if (status != UMI_STATUS_OK) return status;
    (void)memset(out_report, 0, sizeof(*out_report));
    status = umi_application_readiness_report(
        binding->experience, &out_report->features);
    if (status != UMI_STATUS_OK) return status;
    out_report->capabilities = *capabilities;
    out_report->adoption_ready =
        binding->adoption_snapshot.canonical_experience_available &&
        binding->adoption_snapshot.surface_complete;
    out_report->layout_ready =
        binding->adoption_snapshot.layout_projection_complete;
    out_report->tests_ready =
        binding->adoption_snapshot.module_status.tests_available;
    out_report->combined_percent =
        (out_report->features.readiness_percent +
         out_report->capabilities.percent +
         (out_report->adoption_ready ? 100U : 0U) +
         (out_report->layout_ready ? 100U : 0U) +
         (out_report->tests_ready ? 100U : 0U)) / 5U;
    if (!out_report->adoption_ready || !out_report->layout_ready ||
        !out_report->tests_ready ||
        out_report->capabilities.state == UMI_APPLICATION_PRODUCTION_BLOCKED)
        out_report->state = UMI_APPLICATION_PRODUCTION_BLOCKED;
    else if (out_report->capabilities.state ==
             UMI_APPLICATION_PRODUCTION_DEGRADED ||
             out_report->features.planned_count > 0U ||
             out_report->features.foundation_count > 0U)
        out_report->state = UMI_APPLICATION_PRODUCTION_DEGRADED;
    else
        out_report->state = UMI_APPLICATION_PRODUCTION_READY;
    return UMI_STATUS_OK;
}

