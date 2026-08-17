/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_diagnostic_problem_model.c
 *
 * PURPOSE:
 *   Verify the toolkit-neutral Problems model supports deterministic
 *   reconciliation, filtering, grouping, sorting, selection and lifecycle
 *   summaries.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "diagnostic_platform_test_support.h"

static UmiDiagnosticProblem problem_from_finding(
    UmiDiagnosticProviderFinding finding,
    UmiDiagnosticBaselineState baseline,
    UmiDiagnosticSeverity effective,
    int suppressed)
{
    UmiDiagnosticProblem problem;
    (void)memset(&problem, 0, sizeof(problem));
    problem.struct_size = (uint32_t)sizeof(problem);
    problem.api_version = UMI_DIAGNOSTIC_PROBLEM_MODEL_API_VERSION;
    problem.finding = finding;
    problem.original_severity = finding.diagnostic.severity;
    problem.effective_severity = effective;
    problem.baseline_state = baseline;
    problem.first_seen_run_id = finding.run_id;
    problem.last_seen_run_id = finding.run_id;
    problem.active = 1;
    problem.fixable = finding.has_fix;
    if (suppressed) {
        problem.suppression.struct_size =
            (uint32_t)sizeof(problem.suppression);
        problem.suppression.api_version = UMI_DIAGNOSTIC_SUPPRESSION_API_VERSION;
        problem.suppression.matched = 1;
        problem.suppression.suppressed = 1;
        problem.suppression.kind = UMI_DIAGNOSTIC_SUPPRESSION_EXTERNAL;
        problem.suppression.status = UMI_DIAGNOSTIC_SUPPRESSION_ACCEPTED;
        test_copy_text(problem.suppression.reason,
                       sizeof(problem.suppression.reason), "accepted risk");
        test_copy_text(problem.suppression.owner,
                       sizeof(problem.suppression.owner), "quality-team");
    }
    return problem;
}

int main(void)
{
    UmiDiagnosticProblemModel *model = NULL;
    UmiDiagnosticProblemQuery query;
    UmiDiagnosticProblemQueryResult result;
    UmiDiagnosticProblemModelSnapshot snapshot;
    UmiDiagnosticProviderFinding first = test_finding(
        "compiler.gcc", "problem-1", UMI_DIAGNOSTIC_WARNING, "W1",
        "warning one", "src/a.c", 3U, 10U, 1);
    UmiDiagnosticProviderFinding second = test_finding(
        "linter.native", "problem-2", UMI_DIAGNOSTIC_ERROR, "E2",
        "error two", "src/b.c", 8U, 10U, 0);
    UmiDiagnosticProblem first_problem = problem_from_finding(
        first, UMI_DIAGNOSTIC_BASELINE_NEW, UMI_DIAGNOSTIC_ERROR, 0);
    UmiDiagnosticProblem second_problem = problem_from_finding(
        second, UMI_DIAGNOSTIC_BASELINE_UNCHANGED, UMI_DIAGNOSTIC_ERROR, 1);
    size_t resolved = 0U;

    assert(umi_diagnostic_problem_model_create(&model) == UMI_STATUS_OK);
    assert(umi_diagnostic_problem_model_begin_provider_run(
               model, "compiler.gcc", 10U) == UMI_STATUS_OK);
    assert(umi_diagnostic_problem_model_upsert(model, &first_problem) ==
           UMI_STATUS_OK);
    assert(umi_diagnostic_problem_model_upsert(model, &second_problem) ==
           UMI_STATUS_OK);
    assert(umi_diagnostic_problem_model_select(model, first.fingerprint, 1) ==
           UMI_STATUS_OK);

    umi_diagnostic_problem_query_init(&query);
    query.include_suppressed = 0;
    query.baseline_mask = UINT32_C(1) << UMI_DIAGNOSTIC_BASELINE_NEW;
    query.only_fixable = 1;
    assert(umi_diagnostic_problem_query_execute(model, &query, &result) ==
           UMI_STATUS_OK);
    assert(result.count == 1U);
    assert(result.items[0].selected);
    assert(strcmp(result.items[0].finding.diagnostic.id, "problem-1") == 0);

    assert(umi_diagnostic_problem_model_finish_provider_run(
               model, "compiler.gcc", 11U, &resolved) == UMI_STATUS_OK);
    assert(resolved == 1U);
    assert(umi_diagnostic_problem_model_snapshot(model, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.problem_count == 2U);
    assert(snapshot.resolved_count == 1U);
    assert(snapshot.suppressed_count == 1U);
    assert(snapshot.fixable_count == 1U);

    umi_diagnostic_problem_model_destroy(model);
    return 0;
}
