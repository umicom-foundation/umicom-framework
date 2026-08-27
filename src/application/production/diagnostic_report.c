/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/production/diagnostic_report.c
 *
 * PURPOSE:
 *   Implement one bounded part of the Framework-owned application production
 *   control plane while product and frontend code remain independently owned.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/production/diagnostic_report.h"

#include <stdio.h>
#include <string.h>

static UmiStatus append_entry(
    UmiApplicationProductionDiagnosticReport *report,
    UmiApplicationProductionDiagnosticSeverity severity,
    const char *code, const char *subject, const char *message)
{
    UmiStatus status;
    if (report->count >= UMI_APPLICATION_PRODUCTION_MAX_DIAGNOSTICS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    status = umi_application_production_diagnostic_entry_set(
        &report->entries[report->count], severity, code, subject, message);
    if (status != UMI_STATUS_OK) return status;
    report->warning_count +=
        (size_t)(severity == UMI_APPLICATION_PRODUCTION_DIAGNOSTIC_WARNING);
    report->error_count +=
        (size_t)(severity == UMI_APPLICATION_PRODUCTION_DIAGNOSTIC_ERROR);
    report->count += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_application_production_diagnostic_report_build(
    const UmiApplicationProductionRuntime *runtime,
    UmiApplicationProductionDiagnosticReport *out_report)
{
    char message[UMI_APPLICATION_PRODUCTION_TEXT_CAPACITY];
    int written;
    UmiStatus status;
    if (runtime == NULL || out_report == NULL || !runtime->initialised)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_report, 0, sizeof(*out_report));
    written = snprintf(message, sizeof(message),
        "%zu panels, %zu layouts, %zu features, readiness %u%%",
        runtime->panels.count, runtime->layouts.count,
        runtime->features.count, runtime->readiness.combined_percent);
    if (written < 0 || (size_t)written >= sizeof(message))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    status = append_entry(out_report,
        UMI_APPLICATION_PRODUCTION_DIAGNOSTIC_INFO,
        "production.summary", runtime->binding.experience->application_id,
        message);
    if (status != UMI_STATUS_OK) return status;
    if (runtime->panels.uncovered_count > 0U) {
        status = append_entry(out_report,
            UMI_APPLICATION_PRODUCTION_DIAGNOSTIC_ERROR,
            "production.panels.uncovered",
            runtime->binding.experience->application_id,
            "One or more declared panels lack a Framework component.");
        if (status != UMI_STATUS_OK) return status;
    }
    if (!runtime->manifest_drift.compatible) {
        status = append_entry(out_report,
            UMI_APPLICATION_PRODUCTION_DIAGNOSTIC_ERROR,
            "production.manifest.drift",
            runtime->binding.experience->application_id,
            "Observed product contribution differs from its contract.");
        if (status != UMI_STATUS_OK) return status;
    }
    if (runtime->features.application_work_count > 0U) {
        status = append_entry(out_report,
            UMI_APPLICATION_PRODUCTION_DIAGNOSTIC_WARNING,
            "production.application.work",
            runtime->binding.experience->application_id,
            "Thin application-owned implementation work remains.");
        if (status != UMI_STATUS_OK) return status;
    }
    if (runtime->features.external_adapter_work_count > 0U)
        return append_entry(out_report,
            UMI_APPLICATION_PRODUCTION_DIAGNOSTIC_WARNING,
            "production.adapter.work",
            runtime->binding.experience->application_id,
            "External adapter implementation or evidence remains.");
    return UMI_STATUS_OK;
}

