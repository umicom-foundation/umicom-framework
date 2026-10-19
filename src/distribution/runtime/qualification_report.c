/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/qualification_report.c
 * Purpose: Render complete bounded evidence with unambiguous record framing.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "qualification_internal.h"
#include "umicom/distribution/runtime/qualification_report.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

typedef struct ReportWriter { char *output; size_t used; UmiStatus status; } ReportWriter;
static void Bytes(ReportWriter *writer, const char *text, size_t size)
{
    if (writer->status != UMI_STATUS_OK) return;
    if (size > UMI_QUALIFICATION_REPORT_LIMIT - writer->used) {
        writer->status = UMI_STATUS_CAPACITY_EXCEEDED;
        return;
    }
    if (writer->output != NULL) memcpy(writer->output + writer->used, text, size);
    writer->used += size;
}
static void Text(ReportWriter *writer, const char *text) { Bytes(writer, text, strlen(text)); }
static void Number(ReportWriter *writer, uint64_t number)
{
    char text[32];
    int length = snprintf(text, sizeof(text), "%" PRIu64, number);
    if (length < 0 || (size_t)length >= sizeof(text)) { writer->status = UMI_STATUS_INTERNAL_ERROR; return; }
    Bytes(writer, text, (size_t)length);
}
static void Signed(ReportWriter *writer, int number)
{
    char text[32];
    int length = snprintf(text, sizeof(text), "%d", number);
    if (length < 0 || (size_t)length >= sizeof(text)) { writer->status = UMI_STATUS_INTERNAL_ERROR; return; }
    Bytes(writer, text, (size_t)length);
}
static void FieldNumber(ReportWriter *writer, uint64_t number) { Text(writer, "\t"); Number(writer, number); }
static void Hex(ReportWriter *writer, const char *text)
{
    static const char digits[] = "0123456789abcdef";
    for (const unsigned char *p = (const unsigned char *)text; *p != 0U && writer->status == UMI_STATUS_OK; ++p) {
        char pair[2] = {digits[*p >> 4U], digits[*p & 15U]};
        Bytes(writer, pair, sizeof(pair));
    }
}
const char *UmiReleaseQualificationOutcomeText(UmiReleaseQualificationOutcome outcome)
{
    switch (outcome) {
    case UMI_QUALIFICATION_NOT_RUN: return "not-run";
    case UMI_QUALIFICATION_PASSED: return "passed";
    case UMI_QUALIFICATION_FAILED: return "failed";
    case UMI_QUALIFICATION_SKIPPED: return "skipped";
    case UMI_QUALIFICATION_CANCELLED: return "cancelled";
    case UMI_QUALIFICATION_TIMED_OUT: return "timed-out";
    case UMI_QUALIFICATION_DISABLED: return "disabled";
    }
    return "unknown";
}
const char *UmiReleaseQualificationBlockerText(UmiReleaseQualificationBlocker blocker)
{
    switch (blocker) {
    case UMI_QUALIFICATION_OPEN: return "Attempt is still open";
    case UMI_QUALIFICATION_BUILD_MISSING: return "Build evidence is missing";
    case UMI_QUALIFICATION_BUILD_NOT_PASSED: return "Build did not pass";
    case UMI_QUALIFICATION_TESTS_MISSING: return "Selected tests lack results";
    case UMI_QUALIFICATION_TESTS_NOT_PASSED: return "Selected tests did not all pass";
    case UMI_QUALIFICATION_NO_TEST_COVERAGE: return "No tests cover this selection";
    case UMI_QUALIFICATION_SOURCE_UNOBSERVED: return "Source was not observed after execution";
    case UMI_QUALIFICATION_SOURCE_CHANGED: return "Observed source bytes changed";
    case UMI_QUALIFICATION_CONTEXT_CHANGED: return "Configured inventory changed";
    case UMI_QUALIFICATION_TOOLCHAIN_UNOBSERVED: return "Toolchain was not observed after execution";
    case UMI_QUALIFICATION_TOOLCHAIN_CHANGED: return "Toolchain identity changed";
    case UMI_QUALIFICATION_TEST_PROCESS_NOT_PASSED: return "Test runner did not pass";
    case UMI_QUALIFICATION_EXECUTION_INTERRUPTED: return "Qualification execution was interrupted";
    }
    return "Unknown";
}
static void Result(ReportWriter *writer, const UmiReleaseQualificationResult *result)
{
    Text(writer, UmiReleaseQualificationOutcomeText(result->outcome));
    FieldNumber(writer, (uint64_t)result->status);
    Text(writer, "\t");
    if (result->exitCodeKnown) Signed(writer, result->exitCode);
    else Text(writer, "-");
    FieldNumber(writer, result->durationMs);
    Text(writer, "\t"); Text(writer, result->logSha256); Text(writer, "\n");
}
static void Render(const UmiReleaseQualification *run, ReportWriter *writer)
{
    UmiReleaseQualificationSummary summary;
    writer->status = UmiReleaseQualificationDescribe(run, &summary);
    if (writer->status != UMI_STATUS_OK) return;
    Text(writer, "UMICOM-QUALIFICATION\t1\nidentity\t");
    Text(writer, run->identity.execution); Text(writer, "\t"); Text(writer, run->identity.plan);
    Text(writer, "\ninventory\t"); Text(writer, run->inventory);
    Text(writer, "\nsource-before\t"); Text(writer, run->source);
    Text(writer, "\ntoolchain-before\t"); Text(writer, run->toolchain);
    Text(writer, "\nsource-after\t"); Text(writer, run->afterSource);
    Text(writer, "\ninventory-after\t"); Text(writer, run->afterInventory);
    Text(writer, "\ntoolchain-after\t"); Text(writer, run->afterToolchain);
    Text(writer, "\nexecution-status"); FieldNumber(writer, (uint64_t)run->executionStatus);
    Text(writer, "\nselection");
    FieldNumber(writer, (uint64_t)run->selection.changedPaths);
    FieldNumber(writer, run->selection.fallbackReasons);
    Text(writer, "\nsummary");
    uint64_t values[] = {(uint64_t)summary.targets, (uint64_t)summary.tests, (uint64_t)summary.recorded,
        (uint64_t)summary.missing, (uint64_t)summary.passed, (uint64_t)summary.failed,
        (uint64_t)summary.skipped, (uint64_t)summary.notRun, (uint64_t)summary.cancelled,
        (uint64_t)summary.timedOut, (uint64_t)summary.disabled, summary.blockers, summary.closed ? 1U : 0U,
        summary.buildRecorded ? 1U : 0U, summary.readyForOwnerReview ? 1U : 0U,
        summary.testProcessRecorded ? 1U : 0U};
    for (size_t i = 0U; i < sizeof(values) / sizeof(values[0]); ++i) FieldNumber(writer, values[i]);
    Text(writer, "\nbuild\t");
    if (run->buildRecorded) Result(writer, &run->build);
    else Text(writer, "missing\n");
    Text(writer, "test-process\t");
    if (run->testProcessRecorded) Result(writer, &run->testProcess);
    else Text(writer, "missing\n");
    for (size_t i = 0U; writer->status == UMI_STATUS_OK && i < run->count; ++i) {
        const UmiReleaseImpactItem *item = &run->items[i];
        Text(writer, item->kind == UMI_RELEASE_INVENTORY_TARGET ? "target\t" : "test\t");
        Hex(writer, item->name); FieldNumber(writer, item->reasons);
        if (item->kind == UMI_RELEASE_INVENTORY_TARGET) Text(writer, "\n");
        else {
            Text(writer, "\t");
            const UmiQualificationTest *test = &run->tests[i - run->testStart];
            if (test->recorded) Result(writer, &test->result);
            else Text(writer, "missing\n");
        }
    }
    Text(writer, "end\n");
}
UmiStatus UmiReleaseQualificationFormatReport(const UmiReleaseQualification *run,
    char *output, size_t capacity, size_t *outRequired)
{
    if (run == NULL || outRequired == NULL || (output == NULL && capacity != 0U))
        return UMI_STATUS_INVALID_ARGUMENT;
    ReportWriter measured = {0};
    Render(run, &measured);
    if (measured.status != UMI_STATUS_OK) return measured.status;
    if (measured.used == UMI_QUALIFICATION_REPORT_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t required = measured.used + 1U;
    if (output != NULL) {
        if (capacity < required) return UMI_STATUS_CAPACITY_EXCEEDED;
        /* The first pass validates size before changing caller storage. Under
         * serialized access the second pass emits the same immutable bytes. */
        ReportWriter written = {.output = output};
        Render(run, &written);
        if (written.status != UMI_STATUS_OK) return written.status;
        output[written.used] = '\0';
    }
    *outRequired = required;
    return UMI_STATUS_OK;
}
