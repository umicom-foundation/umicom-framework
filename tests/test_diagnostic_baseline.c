/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_diagnostic_baseline.c
 *
 * PURPOSE:
 *   Verify baseline capture and comparison classify new, unchanged, updated
 *   and absent diagnostics deterministically.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "diagnostic_platform_test_support.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDiagnosticBaseline *baseline = NULL;
    UmiDiagnosticProviderBatch *batch = NULL;
    UmiDiagnosticBaselineDecision decision;
    UmiDiagnosticBaselineDiff diff;
    UmiDiagnosticBaselineEntry absent;
    UmiDiagnosticProviderFinding first = test_finding(
        "compiler.gcc", "base-1", UMI_DIAGNOSTIC_WARNING, "W1",
        "first warning", "src/a.c", 3U, 1U, 0);
    UmiDiagnosticProviderFinding second = test_finding(
        "compiler.gcc", "base-2", UMI_DIAGNOSTIC_ERROR, "E2",
        "second error", "src/b.c", 9U, 1U, 0);
    UmiDiagnosticProviderFinding changed = first;
    UmiDiagnosticProviderFinding added = test_finding(
        "compiler.gcc", "base-3", UMI_DIAGNOSTIC_WARNING, "W3",
        "new warning", "src/c.c", 12U, 2U, 0);

    assert(umi_diagnostic_provider_batch_create(&batch) == UMI_STATUS_OK);
    assert(umi_diagnostic_provider_batch_upsert(batch, &first) == UMI_STATUS_OK);
    assert(umi_diagnostic_provider_batch_upsert(batch, &second) == UMI_STATUS_OK);
    assert(umi_diagnostic_baseline_create(&baseline) == UMI_STATUS_OK);
    assert(umi_diagnostic_baseline_capture_batch(baseline, batch) ==
           UMI_STATUS_OK);

    changed.run_id = 2U;
    test_copy_text(changed.diagnostic.message, sizeof(changed.diagnostic.message),
                   "first warning changed");
    changed.content_fingerprint =
        umi_diagnostic_provider_content_fingerprint(&changed);
    assert(umi_diagnostic_baseline_begin_comparison(baseline) == UMI_STATUS_OK);
    assert(umi_diagnostic_baseline_compare(baseline, &changed, &decision) ==
           UMI_STATUS_OK);
    assert(decision.state == UMI_DIAGNOSTIC_BASELINE_UPDATED);
    assert(decision.content_changed);
    assert(umi_diagnostic_baseline_compare(baseline, &added, &decision) ==
           UMI_STATUS_OK);
    assert(decision.state == UMI_DIAGNOSTIC_BASELINE_NEW);
    assert(umi_diagnostic_baseline_finish_comparison(baseline, &diff) ==
           UMI_STATUS_OK);
    assert(diff.updated_count == 1U);
    assert(diff.new_count == 1U);
    assert(diff.absent_count == 1U);
    assert(umi_diagnostic_baseline_absent_at(baseline, 0U, &absent) ==
           UMI_STATUS_OK);
    assert(strcmp(absent.diagnostic_id, "base-2") == 0);

    umi_diagnostic_baseline_destroy(baseline);
    umi_diagnostic_provider_batch_destroy(batch);
    return 0;
}
