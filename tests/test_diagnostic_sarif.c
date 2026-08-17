/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_diagnostic_sarif.c
 *
 * PURPOSE:
 *   Verify dependency-free SARIF 2.1.0 export, validation and import preserve
 *   portable diagnostic evidence and source locations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "diagnostic_platform_test_support.h"

int main(void)
{
    UmiDiagnosticProblemModel *model = NULL;
    UmiDiagnosticProviderBatch *imported = NULL;
    UmiDiagnosticProviderFinding finding = test_finding(
        "linter.native", "sarif-1", UMI_DIAGNOSTIC_WARNING, "U100",
        "unsafe API call", "src/sarif.c", 27U, 77U, 1);
    UmiDiagnosticProblem problem;
    UmiDiagnosticSarifRunDescriptor run =
        umi_diagnostic_sarif_run_default("Umicom Analyzer");
    UmiDiagnosticSarifExportOptions options =
        umi_diagnostic_sarif_export_options_default();
    UmiDiagnosticProviderDescriptor provider;
    UmiDiagnosticSarifImportSummary summary;
    UmiDiagnosticProviderFinding round_trip;
    char *json = NULL;
    size_t json_size = 0U;
    char validation[128];

    (void)memset(&problem, 0, sizeof(problem));
    problem.struct_size = (uint32_t)sizeof(problem);
    problem.api_version = UMI_DIAGNOSTIC_PROBLEM_MODEL_API_VERSION;
    problem.finding = finding;
    problem.original_severity = UMI_DIAGNOSTIC_WARNING;
    problem.effective_severity = UMI_DIAGNOSTIC_ERROR;
    problem.baseline_state = UMI_DIAGNOSTIC_BASELINE_NEW;
    problem.last_seen_run_id = 77U;
    problem.active = 1;
    problem.fixable = 1;
    problem.suppression.struct_size = (uint32_t)sizeof(problem.suppression);
    problem.suppression.api_version = UMI_DIAGNOSTIC_SUPPRESSION_API_VERSION;

    assert(umi_diagnostic_problem_model_create(&model) == UMI_STATUS_OK);
    assert(umi_diagnostic_problem_model_upsert(model, &problem) == UMI_STATUS_OK);
    test_copy_text(run.tool_version, sizeof(run.tool_version), "1.0");
    test_copy_text(run.information_uri, sizeof(run.information_uri),
                   "https://umicom.org/analyzer");
    test_copy_text(run.automation_id, sizeof(run.automation_id),
                   "workspace/build/77");
    run.run_id = 77U;
    run.exit_code = 1;
    run.successful = 0;
    options.pretty = 1;
    assert(umi_diagnostic_sarif_export(model, &run, &options,
                                       &json, &json_size) == UMI_STATUS_OK);
    assert(json != NULL);
    assert(json_size > 100U);
    assert(strstr(json, "\"version\":\"2.1.0\"") != NULL);
    assert(strstr(json, "umicomFingerprint/v1") != NULL);
    assert(strstr(json, "artifactChanges") != NULL);
    assert(umi_diagnostic_sarif_validate(json, json_size, validation,
                                         sizeof(validation)) == UMI_STATUS_OK);

    assert(umi_diagnostic_provider_batch_create(&imported) == UMI_STATUS_OK);
    assert(umi_diagnostic_sarif_import(json, json_size, "sarif.import",
                                       &provider, imported, &summary) ==
           UMI_STATUS_OK);
    assert(summary.run_count == 1U);
    assert(summary.result_count == 1U);
    assert(summary.fix_count == 1U);
    assert(strcmp(provider.tool_name, "Umicom Analyzer") == 0);
    assert(umi_diagnostic_provider_batch_at(imported, 0U, &round_trip) ==
           UMI_STATUS_OK);
    assert(strcmp(round_trip.diagnostic.code, "U100") == 0);
    assert(strcmp(round_trip.diagnostic.uri, "src/sarif.c") == 0);
    assert(round_trip.has_fix);
    assert(strcmp(round_trip.replacement_text, "safe_call()") == 0);
    assert(round_trip.baseline_hint == UMI_DIAGNOSTIC_PROVIDER_BASELINE_NEW);

    umi_diagnostic_sarif_free(json);
    umi_diagnostic_provider_batch_destroy(imported);
    umi_diagnostic_problem_model_destroy(model);
    return 0;
}
