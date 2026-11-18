/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/uat_workflow.c
 *
 * PURPOSE:
 *   Coordinate heterogeneous test adapters with bounded owned evidence and
 *   cleanup that cannot erase a failed or incomplete application workflow.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/testing/uat_workflow.h"
#include <stdlib.h>
#include <string.h>

struct UmiUatReport {
    UmiUatSummary summary;
    UmiUatStep steps[UMI_UAT_MAX_STEPS];
    UmiUatFinding findings[UMI_UAT_MAX_STEPS];
};

/* The legacy case validator checks required fields. Establish bounded strings,
 * enum values and a deadline before it or an adapter reads those fields. */
static int ValidCase(const UmiTestCase *test)
{
    return memchr(test->test_id, 0, sizeof(test->test_id)) != NULL &&
        memchr(test->name, 0, sizeof(test->name)) != NULL &&
        memchr(test->labels, 0, sizeof(test->labels)) != NULL &&
        memchr(test->program, 0, sizeof(test->program)) != NULL &&
        memchr(test->working_directory, 0, sizeof(test->working_directory)) != NULL &&
        memchr(test->argument_text, 0, sizeof(test->argument_text)) != NULL &&
        (test->enabled == 0 || test->enabled == 1) && test->timeout_ms != 0U &&
        test->kind >= UMI_TEST_KIND_UNIT && test->kind <= UMI_TEST_KIND_EXTERNAL &&
        umi_test_case_validate(test, NULL, 0U) == UMI_STATUS_OK;
}

/* A copied plan protects the remaining steps when an adapter edits its source
 * configuration. No external code runs until every row is admitted. */
static UmiStatus ValidatePlan(const UmiUatStep *steps, size_t count)
{
    int body = 0, cleanup = 0, assertions = 0;
    for (size_t i = 0U; i < count; ++i) {
        const UmiUatPhase phase = steps[i].phase;
        if (!ValidCase(&steps[i].test) || phase < UMI_UAT_SETUP || phase > UMI_UAT_CLEANUP)
            return UMI_STATUS_INVALID_ARGUMENT;
        if (phase == UMI_UAT_SETUP && (body || cleanup)) return UMI_STATUS_INVALID_STATE;
        if (phase != UMI_UAT_CLEANUP && cleanup) return UMI_STATUS_INVALID_STATE;
        if (phase == UMI_UAT_ACTION || phase == UMI_UAT_ASSERT) body = 1;
        if (phase == UMI_UAT_CLEANUP) cleanup = 1;
        if (phase == UMI_UAT_ASSERT) assertions = 1;
        for (size_t j = 0U; j < i; ++j)
            if (strcmp(steps[i].test.test_id, steps[j].test.test_id) == 0)
                return UMI_STATUS_ALREADY_EXISTS;
    }
    return assertions ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}

/* Normalize a broken adapter result to failure. A callback returning OK cannot
 * publish RUNNING, an unterminated buffer or a contradictory passing status. */
static void CheckResult(UmiTestResult *result, UmiStatus status)
{
    if (memchr(result->output, 0, sizeof(result->output)) == NULL) {
        result->output[sizeof(result->output) - 1U] = '\0';
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (status != UMI_STATUS_OK) {
        umi_test_result_finish(result, status, result->exit_code, result->duration_ms);
    } else if (result->state < UMI_TEST_STATE_PASSED || result->state > UMI_TEST_STATE_TIMED_OUT ||
        (result->state == UMI_TEST_STATE_PASSED &&
            (result->status != UMI_STATUS_OK || result->exit_code != 0))) {
        umi_test_result_finish(result, UMI_STATUS_INVALID_STATE, -1, result->duration_ms);
    }
}

UmiStatus UmiUatRun(const UmiUatStep *steps, size_t count, UmiUatExecuteFn execute,
    void *context, UmiCancellationToken *cancellation,
    UmiCancellationToken *cleanupCancellation, UmiUatReport **outReport)
{
    UmiUatReport *report;
    UmiStatus status;
    int blocked = 0, started = 0;
    if (outReport == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outReport = NULL;
    if (steps == NULL || execute == NULL || count == 0U || count > UMI_UAT_MAX_STEPS)
        return UMI_STATUS_INVALID_ARGUMENT;
    report = calloc(1U, sizeof(*report));
    if (report == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(report->steps, steps, count * sizeof(*steps));
    status = ValidatePlan(report->steps, count);
    if (status != UMI_STATUS_OK) { free(report); return status; }
    report->summary.planned = count;
    for (size_t i = 0U; i < count; ++i) {
        const UmiUatStep *step = &report->steps[i];
        UmiUatFinding *finding = &report->findings[i];
        UmiCancellationToken *token = step->phase == UMI_UAT_CLEANUP
            ? cleanupCancellation : cancellation;
        finding->phase = step->phase;
        umi_test_result_init(&finding->result, step->test.test_id, step->test.name);
        if ((step->phase == UMI_UAT_CLEANUP && !started) ||
            (step->phase != UMI_UAT_CLEANUP && blocked)) continue;
        if (umi_cancellation_token_is_requested(token)) {
            finding->result.status = UMI_STATUS_CANCELLED;
            if (step->phase != UMI_UAT_CLEANUP) blocked = 1;
            continue;
        }
        if (!step->test.enabled) {
            finding->result.state = UMI_TEST_STATE_SKIPPED;
            if (step->phase != UMI_UAT_CLEANUP) blocked = 1;
            continue;
        }
        if (step->phase != UMI_UAT_CLEANUP) started = 1;
        finding->attempted = 1;
        status = execute(context, &step->test, token, &finding->result);
        CheckResult(&finding->result, status);
        /* Adapters cannot change the identity of the row in the retained plan. */
        memcpy(finding->result.test_id, step->test.test_id, sizeof(finding->result.test_id));
        memcpy(finding->result.name, step->test.name, sizeof(finding->result.name));
        if (finding->result.state != UMI_TEST_STATE_PASSED && step->phase != UMI_UAT_CLEANUP)
            blocked = 1;
    }
    for (size_t i = 0U; i < count; ++i) {
        const UmiUatFinding *finding = &report->findings[i];
        if (finding->attempted) ++report->summary.attempted;
        if (finding->result.state == UMI_TEST_STATE_PASSED) ++report->summary.passed;
        else if (finding->result.state == UMI_TEST_STATE_NOT_RUN) ++report->summary.notRun;
        else if (finding->result.state == UMI_TEST_STATE_SKIPPED) ++report->summary.skipped;
        else {
            ++report->summary.failed;
            if (finding->phase == UMI_UAT_CLEANUP) ++report->summary.cleanupFailed;
        }
    }
    report->summary.cancellationRequested = umi_cancellation_token_is_requested(cancellation) ||
        umi_cancellation_token_is_requested(cleanupCancellation);
    report->summary.accepted = report->summary.passed == count && !report->summary.cancellationRequested;
    *outReport = report;
    return UMI_STATUS_OK;
}

void UmiUatReportDestroy(UmiUatReport *report) { free(report); }
UmiStatus UmiUatReportSummary(const UmiUatReport *report, UmiUatSummary *outSummary)
{
    if (outSummary == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(outSummary, 0, sizeof(*outSummary));
    if (report == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outSummary = report->summary;
    return UMI_STATUS_OK;
}
UmiStatus UmiUatReportFinding(const UmiUatReport *report, size_t index, UmiUatFinding *outFinding)
{
    if (outFinding == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(outFinding, 0, sizeof(*outFinding));
    if (report == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= report->summary.planned) return UMI_STATUS_NOT_FOUND;
    *outFinding = report->findings[index];
    return UMI_STATUS_OK;
}
