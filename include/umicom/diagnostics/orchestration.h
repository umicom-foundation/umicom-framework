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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * List the named diagnostic orchestration state values accepted by this public contract.
 */
typedef enum UmiDiagnosticOrchestrationState {
    UMI_DIAGNOSTIC_ORCHESTRATION_IDLE = 1,
    UMI_DIAGNOSTIC_ORCHESTRATION_RUNNING = 2,
    UMI_DIAGNOSTIC_ORCHESTRATION_COMPLETED = 3,
    UMI_DIAGNOSTIC_ORCHESTRATION_CANCELLED = 4,
    UMI_DIAGNOSTIC_ORCHESTRATION_FAILED = 5
} UmiDiagnosticOrchestrationState;

/**
 * Represent the diagnostic orchestration config data shared with callers of this public
 * contract.
 */
typedef struct UmiDiagnosticOrchestrationConfig {
    int mirror_to_diagnostic_model;
    int retain_suppressed_in_diagnostic_model;
    int retain_disabled_in_problem_model;
    int resolve_missing_provider_findings;
} UmiDiagnosticOrchestrationConfig;

/**
 * Represent the diagnostic orchestration snapshot data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the diagnostic orchestration data shared with callers of this public contract.
 */
typedef struct UmiDiagnosticOrchestration UmiDiagnosticOrchestration;

/**
 * Provide the diagnostic orchestration config default operation used by this module and
 * its client applications.
 */
UmiDiagnosticOrchestrationConfig umi_diagnostic_orchestration_config_default(void);
/**
 * Initialise diagnostic orchestration from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_diagnostic_orchestration_create(
    const UmiDiagnosticOrchestrationConfig *config,
    UmiDiagnosticOrchestration **out_orchestration);
/**
 * Release or reset state held by diagnostic orchestration so the same storage can be
 * reused safely.
 */
void umi_diagnostic_orchestration_destroy(
    UmiDiagnosticOrchestration *orchestration);
/**
 * Provide the diagnostic orchestration bind model operation used by this module and its
 * client applications.
 */
UmiStatus umi_diagnostic_orchestration_bind_model(
    UmiDiagnosticOrchestration *orchestration,
    UmiDiagnosticModel *diagnostic_model);
/**
 * Perform diagnostic orchestration begin provider through the module contract so client
 * applications do not duplicate its policy.
 */
UmiStatus umi_diagnostic_orchestration_begin_provider_run(
    UmiDiagnosticOrchestration *orchestration,
    const char *provider_id,
    uint64_t run_id);
/**
 * Provide the diagnostic orchestration ingest batch operation used by this module and its
 * client applications.
 */
UmiStatus umi_diagnostic_orchestration_ingest_batch(
    UmiDiagnosticOrchestration *orchestration,
    const UmiDiagnosticProviderBatch *batch,
    uint64_t timestamp_ns);
/**
 * Perform diagnostic orchestration finish provider through the module contract so client
 * applications do not duplicate its policy.
 */
UmiStatus umi_diagnostic_orchestration_finish_provider_run(
    UmiDiagnosticOrchestration *orchestration);
/**
 * Provide the diagnostic orchestration cancel operation used by this module and its client
 * applications.
 */
UmiStatus umi_diagnostic_orchestration_cancel(
    UmiDiagnosticOrchestration *orchestration);
/**
 * Provide the diagnostic orchestration reapply operation used by this module and its
 * client applications.
 */
UmiStatus umi_diagnostic_orchestration_reapply(
    UmiDiagnosticOrchestration *orchestration,
    uint64_t timestamp_ns);
/**
 * Provide the diagnostic orchestration capture baseline operation used by this module and
 * its client applications.
 */
UmiStatus umi_diagnostic_orchestration_capture_baseline(
    UmiDiagnosticOrchestration *orchestration);
/**
 * Provide the diagnostic orchestration import sarif operation used by this module and its
 * client applications.
 */
UmiStatus umi_diagnostic_orchestration_import_sarif(
    UmiDiagnosticOrchestration *orchestration,
    const char *json,
    size_t json_size,
    const char *provider_id,
    uint64_t timestamp_ns,
    UmiDiagnosticSarifImportSummary *out_summary);
/**
 * Provide the diagnostic orchestration export sarif operation used by this module and its
 * client applications.
 */
UmiStatus umi_diagnostic_orchestration_export_sarif(
    const UmiDiagnosticOrchestration *orchestration,
    const UmiDiagnosticSarifRunDescriptor *run,
    const UmiDiagnosticSarifExportOptions *options,
    char **out_json,
    size_t *out_size);
/**
 * Provide the diagnostic orchestration snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_diagnostic_orchestration_snapshot(
    const UmiDiagnosticOrchestration *orchestration,
    UmiDiagnosticOrchestrationSnapshot *out_snapshot);

/**
 * Provide the diagnostic orchestration providers operation used by this module and its
 * client applications.
 */
UmiDiagnosticProviderRegistry *umi_diagnostic_orchestration_providers(
    UmiDiagnosticOrchestration *orchestration);
/**
 * Provide the diagnostic orchestration severity policy operation used by this module and
 * its client applications.
 */
UmiDiagnosticSeverityPolicy *umi_diagnostic_orchestration_severity_policy(
    UmiDiagnosticOrchestration *orchestration);
/**
 * Provide the diagnostic orchestration suppressions operation used by this module and its
 * client applications.
 */
UmiDiagnosticSuppressionSet *umi_diagnostic_orchestration_suppressions(
    UmiDiagnosticOrchestration *orchestration);
/**
 * Provide the diagnostic orchestration baseline operation used by this module and its
 * client applications.
 */
UmiDiagnosticBaseline *umi_diagnostic_orchestration_baseline(
    UmiDiagnosticOrchestration *orchestration);
/**
 * Provide the diagnostic orchestration problems operation used by this module and its
 * client applications.
 */
UmiDiagnosticProblemModel *umi_diagnostic_orchestration_problems(
    UmiDiagnosticOrchestration *orchestration);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DIAGNOSTICS_ORCHESTRATION_H */
