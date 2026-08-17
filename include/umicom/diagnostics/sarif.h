/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/diagnostics/sarif.h
 *
 * PURPOSE:
 *   Import and export the portable subset of SARIF 2.1.0 required by Umicom
 *   diagnostics. The implementation preserves tool, rule, location, baseline,
 *   suppression, fingerprint and fix evidence without depending on a GUI or an
 *   external JSON library.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DIAGNOSTICS_SARIF_H
#define UMICOM_DIAGNOSTICS_SARIF_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/diagnostics/problem_model.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DIAGNOSTIC_SARIF_API_VERSION 1U
#define UMI_DIAGNOSTIC_SARIF_TOOL_CAPACITY 128U
#define UMI_DIAGNOSTIC_SARIF_VERSION_CAPACITY 64U
#define UMI_DIAGNOSTIC_SARIF_AUTOMATION_ID_CAPACITY 256U
#define UMI_DIAGNOSTIC_SARIF_URI_CAPACITY 1024U

typedef struct UmiDiagnosticSarifRunDescriptor {
    uint32_t struct_size;
    uint32_t api_version;
    char tool_name[UMI_DIAGNOSTIC_SARIF_TOOL_CAPACITY];
    char tool_version[UMI_DIAGNOSTIC_SARIF_VERSION_CAPACITY];
    char information_uri[UMI_DIAGNOSTIC_SARIF_URI_CAPACITY];
    char automation_id[UMI_DIAGNOSTIC_SARIF_AUTOMATION_ID_CAPACITY];
    uint64_t run_id;
    uint64_t started_timestamp_ns;
    uint64_t ended_timestamp_ns;
    int32_t exit_code;
    int successful;
} UmiDiagnosticSarifRunDescriptor;

typedef struct UmiDiagnosticSarifExportOptions {
    int pretty;
    int include_suppressed;
    int include_resolved;
    int include_disabled;
    int include_fixes;
    size_t maximum_results;
} UmiDiagnosticSarifExportOptions;

typedef struct UmiDiagnosticSarifImportSummary {
    uint32_t struct_size;
    uint32_t api_version;
    size_t run_count;
    size_t result_count;
    size_t suppressed_count;
    size_t fix_count;
    size_t skipped_count;
    uint64_t run_id;
} UmiDiagnosticSarifImportSummary;

UmiDiagnosticSarifRunDescriptor umi_diagnostic_sarif_run_default(
    const char *tool_name);
UmiDiagnosticSarifExportOptions umi_diagnostic_sarif_export_options_default(void);
UmiStatus umi_diagnostic_sarif_export(
    const UmiDiagnosticProblemModel *model,
    const UmiDiagnosticSarifRunDescriptor *run,
    const UmiDiagnosticSarifExportOptions *options,
    char **out_json,
    size_t *out_size);
UmiStatus umi_diagnostic_sarif_import(
    const char *json,
    size_t json_size,
    const char *provider_id,
    UmiDiagnosticProviderDescriptor *out_provider,
    UmiDiagnosticProviderBatch *out_batch,
    UmiDiagnosticSarifImportSummary *out_summary);
UmiStatus umi_diagnostic_sarif_validate(
    const char *json,
    size_t json_size,
    char *out_message,
    size_t message_capacity);
void umi_diagnostic_sarif_free(void *memory);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DIAGNOSTICS_SARIF_H */
