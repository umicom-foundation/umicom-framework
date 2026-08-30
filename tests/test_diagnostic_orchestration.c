/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_diagnostic_orchestration.c
 *
 * PURPOSE:
 *   Verify provider-run orchestration applies severity policy, suppressions,
 *   baselines and Problems-model reconciliation in one Framework-owned
 *   lifecycle.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "diagnostic_platform_test_support.h"

int main(void)
{
    UmiDiagnosticOrchestration *orchestration = NULL;
    UmiDiagnosticProviderBatch *batch = NULL;
    UmiDiagnosticProviderDescriptor provider = test_provider("linter.native", 20);
    UmiDiagnosticProviderFinding first = test_finding(
        "linter.native", "orch-1", UMI_DIAGNOSTIC_WARNING, "U100",
        "unsafe API", "src/a.c", 4U, 100U, 1);
    UmiDiagnosticProviderFinding second = test_finding(
        "linter.native", "orch-2", UMI_DIAGNOSTIC_WARNING, "U200",
        "generated warning", "generated/b.c", 8U, 100U, 0);
    UmiDiagnosticSeverityRule promote;
    UmiDiagnosticSuppressionRule suppress;
    UmiDiagnosticOrchestrationSnapshot snapshot;
    UmiDiagnosticProblemModelSnapshot problems;
    UmiDiagnosticProblemQuery query;
    UmiDiagnosticProblemQueryResult results;

    assert(umi_diagnostic_orchestration_create(NULL, &orchestration) ==
           UMI_STATUS_OK);
    assert(umi_diagnostic_provider_registry_upsert(
               umi_diagnostic_orchestration_providers(orchestration),
               &provider) == UMI_STATUS_OK);

    (void)memset(&promote, 0, sizeof(promote));
    promote.struct_size = (uint32_t)sizeof(promote);
    promote.api_version = UMI_DIAGNOSTIC_SEVERITY_POLICY_API_VERSION;
    test_copy_text(promote.id, sizeof(promote.id), "warnings-as-errors");
    test_copy_text(promote.provider_pattern, sizeof(promote.provider_pattern),
                   "linter.*");
    promote.action = UMI_DIAGNOSTIC_SEVERITY_PROMOTE;
    promote.target_severity = UMI_DIAGNOSTIC_WARNING;
    promote.steps = 1U;
    promote.enabled = 1;
    assert(umi_diagnostic_severity_policy_upsert(
               umi_diagnostic_orchestration_severity_policy(orchestration),
               &promote) == UMI_STATUS_OK);

    (void)memset(&suppress, 0, sizeof(suppress));
    suppress.struct_size = (uint32_t)sizeof(suppress);
    suppress.api_version = UMI_DIAGNOSTIC_SUPPRESSION_API_VERSION;
    test_copy_text(suppress.id, sizeof(suppress.id), "generated");
    test_copy_text(suppress.uri_pattern, sizeof(suppress.uri_pattern),
                   "generated/*");
    test_copy_text(suppress.reason, sizeof(suppress.reason),
                   "Generated file is controlled by its generator.");
    test_copy_text(suppress.owner, sizeof(suppress.owner), "build-team");
    suppress.kind = UMI_DIAGNOSTIC_SUPPRESSION_EXTERNAL;
    suppress.status = UMI_DIAGNOSTIC_SUPPRESSION_ACCEPTED;
    suppress.enabled = 1;
    assert(umi_diagnostic_suppression_set_upsert(
               umi_diagnostic_orchestration_suppressions(orchestration),
               &suppress) == UMI_STATUS_OK);

    assert(umi_diagnostic_provider_batch_create(&batch) == UMI_STATUS_OK);
    assert(umi_diagnostic_provider_batch_upsert(batch, &first) == UMI_STATUS_OK);
    assert(umi_diagnostic_provider_batch_upsert(batch, &second) == UMI_STATUS_OK);
    assert(umi_diagnostic_orchestration_begin_provider_run(
               orchestration, "linter.native", 100U) == UMI_STATUS_OK);
    assert(umi_diagnostic_orchestration_ingest_batch(
               orchestration, batch, UINT64_C(9000)) == UMI_STATUS_OK);
    assert(umi_diagnostic_orchestration_finish_provider_run(orchestration) ==
           UMI_STATUS_OK);
    assert(umi_diagnostic_orchestration_snapshot(orchestration, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.accepted_count == 2U);
    assert(snapshot.suppressed_count == 1U);
    assert(snapshot.new_count == 2U);

    assert(umi_diagnostic_problem_model_snapshot(
               umi_diagnostic_orchestration_problems(orchestration),
               &problems) == UMI_STATUS_OK);
    assert(problems.problem_count == 2U);
    assert(problems.by_effective_severity[UMI_DIAGNOSTIC_ERROR] == 2U);
    assert(problems.suppressed_count == 1U);

    assert(umi_diagnostic_orchestration_capture_baseline(orchestration) ==
           UMI_STATUS_OK);
    assert(umi_diagnostic_orchestration_reapply(orchestration,
                                                 UINT64_C(10000)) ==
           UMI_STATUS_OK);
    umi_diagnostic_problem_query_init(&query);
    query.include_suppressed = 1;
    query.baseline_mask = UINT32_C(1) << UMI_DIAGNOSTIC_BASELINE_UNCHANGED;
    assert(umi_diagnostic_problem_query_execute(
               umi_diagnostic_orchestration_problems(orchestration),
               &query, &results) == UMI_STATUS_OK);
    assert(results.total_matches == 2U);

    umi_diagnostic_provider_batch_destroy(batch);
    umi_diagnostic_orchestration_destroy(orchestration);
    return 0;
}
