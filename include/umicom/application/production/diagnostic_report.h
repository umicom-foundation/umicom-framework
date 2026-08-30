/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/production/diagnostic_report.h
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
#ifndef UMICOM_APPLICATION_PRODUCTION_DIAGNOSTIC_REPORT_H
#define UMICOM_APPLICATION_PRODUCTION_DIAGNOSTIC_REPORT_H

#include "umicom/application/production/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/application/production/diagnostic_entry.h"
#include "umicom/application/production/runtime.h"

typedef struct UmiApplicationProductionDiagnosticReport {
    UmiApplicationProductionDiagnosticEntry
        entries[UMI_APPLICATION_PRODUCTION_MAX_DIAGNOSTICS];
    size_t count;
    size_t warning_count;
    size_t error_count;
} UmiApplicationProductionDiagnosticReport;

UmiStatus umi_application_production_diagnostic_report_build(
    const UmiApplicationProductionRuntime *runtime,
    UmiApplicationProductionDiagnosticReport *out_report);

#ifdef __cplusplus
}
#endif
#endif
