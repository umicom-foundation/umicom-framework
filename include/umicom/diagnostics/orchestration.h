/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/diagnostics/orchestration.h
 *
 * PURPOSE:
 *   Coordinate provider runs, severity policy, suppression, baseline
 *   classification, the canonical Problems model and optional mirroring into
 *   the existing immutable diagnostic model. Applications bind views and
 *   commands; all processing behaviour remains Framework-owned.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DIAGNOSTICS_ORCHESTRATION_H
#define UMICOM_DIAGNOSTICS_ORCHESTRATION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/diagnostics/model.h"
#include "umicom/diagnostics/problem_model.h"
#include "umicom/diagnostics/sarif.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DIAGNOSTIC_ORCHESTRATION_API_VERSION 1U

typedef enum UmiDiagnosticOrchestrationState {
    UMI_DIAGNOSTIC_ORCHESTRATION_IDLE = 1,
    UMI_DIAGNOSTIC_ORCHESTRATION_RUNNING = 2,
    UMI_DIAGNOSTIC_ORCHESTRATION_COMPLETED = 3,
    UMI_DIAGNOSTIC_ORCHESTRATION_CANCELLED = 4,
    UMI_DIAGNOSTIC_ORCHESTRATION_FAILED = 5
} UmiDiagnosticOrchestrationState;

typedef struct UmiDiagnosticOrchestrationConfig {
    int mirror_to_diagnostic_model;
    int retain_suppressed_in_diagnostic_model;
    int retain_disabled_in_problem_model;
    int resolve_missing_provider_findings;
} UmiDiagnosticOrchestrationConfig;

typedef struct UmiDiagnosticOrchestrationSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    UmiDiagnosticOrchestrationState state;
    char active_provider_id[UMI_DIAGNOSTIC_PROVIDER_ID_CAPACITY];
    uint64_t active_run_id;
    size_t ingested_count;
    size_t accepted_count;
    size_t suppressed_count;
    size_t disabled_count;
    size_t new_count;
    size_t updated_count;
    size_t resolved_count;
    uint64_t revision;
} UmiDiagnosticOrchestrationSnapshot;

typedef struct UmiDiagnosticOrchestration UmiDiagnosticOrchestration;

UmiDiagnosticOrchestrationConfig umi_diagnostic_orchestration_config_default(void);
UmiStatus umi_diagnostic_orchestration_create(
    const UmiDiagnosticOrchestrationConfig *config,
    UmiDiagnosticOrchestration **out_orchestration);
void umi_diagnostic_orchestration_destroy(
    UmiDiagnosticOrchestration *orchestration);
UmiStatus umi_diagnostic_orchestration_bind_model(
    UmiDiagnosticOrchestration *orchestration,
    UmiDiagnosticModel *diagnostic_model);
UmiStatus umi_diagnostic_orchestration_begin_provider_run(
    UmiDiagnosticOrchestration *orchestration,
    const char *provider_id,
    uint64_t run_id);
UmiStatus umi_diagnostic_orchestration_ingest_batch(
    UmiDiagnosticOrchestration *orchestration,
    const UmiDiagnosticProviderBatch *batch,
    uint64_t timestamp_ns);
UmiStatus umi_diagnostic_orchestration_finish_provider_run(
    UmiDiagnosticOrchestration *orchestration);
UmiStatus umi_diagnostic_orchestration_cancel(
    UmiDiagnosticOrchestration *orchestration);
UmiStatus umi_diagnostic_orchestration_reapply(
    UmiDiagnosticOrchestration *orchestration,
    uint64_t timestamp_ns);
UmiStatus umi_diagnostic_orchestration_capture_baseline(
    UmiDiagnosticOrchestration *orchestration);
UmiStatus umi_diagnostic_orchestration_import_sarif(
    UmiDiagnosticOrchestration *orchestration,
    const char *json,
    size_t json_size,
    const char *provider_id,
    uint64_t timestamp_ns,
    UmiDiagnosticSarifImportSummary *out_summary);
UmiStatus umi_diagnostic_orchestration_export_sarif(
    const UmiDiagnosticOrchestration *orchestration,
    const UmiDiagnosticSarifRunDescriptor *run,
    const UmiDiagnosticSarifExportOptions *options,
    char **out_json,
    size_t *out_size);
UmiStatus umi_diagnostic_orchestration_snapshot(
    const UmiDiagnosticOrchestration *orchestration,
    UmiDiagnosticOrchestrationSnapshot *out_snapshot);

UmiDiagnosticProviderRegistry *umi_diagnostic_orchestration_providers(
    UmiDiagnosticOrchestration *orchestration);
UmiDiagnosticSeverityPolicy *umi_diagnostic_orchestration_severity_policy(
    UmiDiagnosticOrchestration *orchestration);
UmiDiagnosticSuppressionSet *umi_diagnostic_orchestration_suppressions(
    UmiDiagnosticOrchestration *orchestration);
UmiDiagnosticBaseline *umi_diagnostic_orchestration_baseline(
    UmiDiagnosticOrchestration *orchestration);
UmiDiagnosticProblemModel *umi_diagnostic_orchestration_problems(
    UmiDiagnosticOrchestration *orchestration);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DIAGNOSTICS_ORCHESTRATION_H */
