/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/qualification_parse.c
 *
 * PURPOSE:
 *   Reconstruct and validate saved evidence without recreating an executable job.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "qualification_parse_internal.h"
#include "umicom/distribution/runtime/qualification_archive.h"
#include "umicom/distribution/runtime/qualification_report.h"
#include <stdlib.h>
#include <string.h>
static UmiStatus Row(UmiQualificationReader *reader, const char *name,
    UmiQualificationField *fields, size_t expected)
{
    size_t count = 0U;
    UmiStatus status = UmiQualificationReadRow(reader, fields, 20U, &count);
    return status == UMI_STATUS_OK && count == expected && UmiQualificationFieldIs(fields[0], name)
        ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
}
static UmiStatus DigestRow(UmiQualificationReader *reader, const char *name, bool optional, char output[65])
{
    UmiQualificationField fields[20];
    UmiStatus status = Row(reader, name, fields, 2U);
    return status == UMI_STATUS_OK ? UmiQualificationReadDigest(fields[1], optional, output) : status;
}
static UmiStatus ResultRow(UmiQualificationReader *reader, const char *name,
    UmiReleaseQualificationResult *result, bool *recorded)
{
    UmiQualificationField fields[20]; size_t count = 0U;
    UmiStatus status = UmiQualificationReadRow(reader, fields, 20U, &count);
    if (status != UMI_STATUS_OK || count < 2U || !UmiQualificationFieldIs(fields[0], name))
        return UMI_STATUS_PARSE_ERROR;
    return UmiQualificationReadResult(fields + 1, count - 1U, result, recorded);
}
/* Protocol masks name the supported public flags. Keeping these tied to the
 * planner avoids rejecting its own saved reports when a new reason is added,
 * while unknown future bits still require an explicit reader update. */
static UmiStatus ReadMask(UmiQualificationField field, uint32_t allowed, uint64_t *output)
{
    uint64_t value = 0U;
    UmiStatus status = UmiQualificationReadNumber(field, UINT32_MAX, &value);
    if (status != UMI_STATUS_OK) return status;
    if (((uint32_t)value & ~allowed) != 0U) return UMI_STATUS_PARSE_ERROR;
    *output = value;
    return UMI_STATUS_OK;
}
static UmiStatus Prefix(UmiQualificationReader *reader, UmiReleaseQualification *run,
    uint64_t summary[16], char expectedPlan[65])
{
    UmiQualificationField fields[20];
    UmiStatus status = Row(reader, "UMICOM-QUALIFICATION", fields, 2U);
    if (status != UMI_STATUS_OK || !UmiQualificationFieldIs(fields[1], "1")) return UMI_STATUS_PARSE_ERROR;
    status = Row(reader, "identity", fields, 3U);
    if (status == UMI_STATUS_OK) status = UmiQualificationReadDigest(fields[1], false, run->identity.execution);
    if (status == UMI_STATUS_OK) status = UmiQualificationReadDigest(fields[2], false, expectedPlan);
    if (status == UMI_STATUS_OK) status = DigestRow(reader, "inventory", false, run->inventory);
    if (status == UMI_STATUS_OK) status = DigestRow(reader, "source-before", false, run->source);
    if (status == UMI_STATUS_OK) status = DigestRow(reader, "toolchain-before", false, run->toolchain);
    if (status == UMI_STATUS_OK) status = DigestRow(reader, "source-after", true, run->afterSource);
    if (status == UMI_STATUS_OK) status = DigestRow(reader, "inventory-after", true, run->afterInventory);
    if (status == UMI_STATUS_OK) status = DigestRow(reader, "toolchain-after", true, run->afterToolchain);
    if (status == UMI_STATUS_OK) status = Row(reader, "execution-status", fields, 2U);
    uint64_t executionStatus = 0U, changed = 0U, fallback = 0U;
    if (status == UMI_STATUS_OK) status = UmiQualificationReadNumber(fields[1], UMI_STATUS_BUSY, &executionStatus);
    if (status == UMI_STATUS_OK) status = Row(reader, "selection", fields, 3U);
    if (status == UMI_STATUS_OK) status = UmiQualificationReadNumber(fields[1], UMI_RELEASE_IMPACT_CHANGE_LIMIT, &changed);
    /* Retain the former numeric limit for review; named flags also admit an
     * unresolved required test while continuing to refuse unknown bits. */
#if 0
    if (status == UMI_STATUS_OK) status = UmiQualificationReadNumber(fields[2], 63U, &fallback);
#endif
    if (status == UMI_STATUS_OK) status = ReadMask(fields[2],
        UMI_RELEASE_IMPACT_REQUESTED_FULL | UMI_RELEASE_IMPACT_UNMAPPED_PATH |
        UMI_RELEASE_IMPACT_BUILD_CONFIGURATION | UMI_RELEASE_IMPACT_INCOMPLETE_GRAPH |
        UMI_RELEASE_IMPACT_UNOWNED_TEST | UMI_RELEASE_IMPACT_UNRESOLVED_SOURCE |
        UMI_RELEASE_IMPACT_UNMAPPED_TEST, &fallback);
    if (status == UMI_STATUS_OK) status = Row(reader, "summary", fields, 17U);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < 16U; ++i)
        status = UmiQualificationReadNumber(fields[i + 1U], i >= 12U ? 1U :
            i == 11U ? 8191U : UMI_RELEASE_INVENTORY_RECORD_LIMIT, &summary[i]);
    if (status != UMI_STATUS_OK) return status;
    if (summary[0] + summary[1] > UMI_RELEASE_INVENTORY_RECORD_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    run->selection = (UmiReleaseImpactSummary){.changedPaths = (size_t)changed,
        .targets = (size_t)summary[0], .tests = (size_t)summary[1],
        .fallbackReasons = (uint32_t)fallback, .missingTestCoverage = summary[1] == 0U};
    run->testStart = run->selection.targets; run->closed = summary[12] != 0U;
    run->executionStatus = (UmiStatus)executionStatus;
    size_t count = run->selection.targets + run->selection.tests;
    run->items = calloc(count == 0U ? 1U : count, sizeof(*run->items));
    run->tests = calloc(run->selection.tests == 0U ? 1U : run->selection.tests, sizeof(*run->tests));
    if (run->items == NULL || run->tests == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = ResultRow(reader, "build", &run->build, &run->buildRecorded);
    if (status == UMI_STATUS_OK) status = ResultRow(reader, "test-process", &run->testProcess, &run->testProcessRecorded);
    return status;
}
static UmiStatus Items(UmiQualificationReader *reader, UmiReleaseQualification *run)
{
    size_t count = run->selection.targets + run->selection.tests;
    for (size_t i = 0U; i < count; ++i) {
        UmiQualificationField fields[20]; size_t columns = 0U;
        UmiStatus status = UmiQualificationReadRow(reader, fields, 20U, &columns);
        bool test = i >= run->testStart;
        if (status != UMI_STATUS_OK || (test ? columns < 4U : columns != 3U) ||
            !UmiQualificationFieldIs(fields[0], test ? "test" : "target")) return UMI_STATUS_PARSE_ERROR;
        char *name = NULL;
        status = UmiQualificationReadName(reader, fields[1], &name);
        if (status != UMI_STATUS_OK) return status;
        /* Group order makes exact-name lookup deterministic and prevents duplicate
         * rows from inflating test counts. A target and test may share a name. */
        if (i != 0U && i != run->testStart && strcmp(run->items[i - 1U].name, name) >= 0) {
            free(name); return UMI_STATUS_PARSE_ERROR;
        }
        run->items[run->count++] = (UmiReleaseImpactItem){
            test ? UMI_RELEASE_INVENTORY_TEST : UMI_RELEASE_INVENTORY_TARGET, name, 0U};
        uint64_t reasons = 0U;
        /* Required-test selections add a reason beyond the former range.
         * Preserve that range below and read the current symbolic mask. */
#if 0
        status = UmiQualificationReadNumber(fields[2], 127U, &reasons);
#endif
        status = ReadMask(fields[2], UMI_RELEASE_IMPACT_CHANGED_SOURCE |
            UMI_RELEASE_IMPACT_DEPENDENT | UMI_RELEASE_IMPACT_TEST_OWNER |
            UMI_RELEASE_IMPACT_ALWAYS | UMI_RELEASE_IMPACT_FULL |
            UMI_RELEASE_IMPACT_TEST_PREREQUISITE | UMI_RELEASE_IMPACT_TEST_DEPENDENCY |
            UMI_RELEASE_IMPACT_REQUIRED_TEST, &reasons);
        run->items[i].reasons = (uint32_t)reasons;
        if (status == UMI_STATUS_OK && test) {
            UmiQualificationTest *entry = &run->tests[i - run->testStart];
            status = UmiQualificationReadResult(fields + 3, columns - 3U, &entry->result, &entry->recorded);
        }
        if (status != UMI_STATUS_OK) return status;
    }
    UmiQualificationField fields[20];
    UmiStatus status = Row(reader, "end", fields, 1U);
    return status == UMI_STATUS_OK && reader->next == reader->end ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
}
static UmiStatus Validate(UmiReleaseQualification *run, const uint64_t expected[16], const char *expectedPlan)
{
    if ((run->afterSource[0] == '\0') != (run->afterInventory[0] == '\0')) return UMI_STATUS_PARSE_ERROR;
    if (!run->closed && (run->afterSource[0] != '\0' || run->afterToolchain[0] != '\0'))
        return UMI_STATUS_PARSE_ERROR;
    if (run->buildRecorded && run->build.outcome == UMI_QUALIFICATION_PASSED && !run->build.exitCodeKnown)
        return UMI_STATUS_PARSE_ERROR;
    if (run->testProcessRecorded && (!run->buildRecorded || run->selection.tests == 0U ||
        (run->testProcess.outcome == UMI_QUALIFICATION_PASSED && !run->testProcess.exitCodeKnown)))
        return UMI_STATUS_PARSE_ERROR;
    for (size_t i = 0U; i < run->selection.tests; ++i) {
        const UmiQualificationTest *test = &run->tests[i];
        if ((!test->recorded && run->testProcessRecorded) || (test->recorded && !run->buildRecorded) ||
            (test->recorded && test->result.outcome == UMI_QUALIFICATION_PASSED &&
             !test->result.exitCodeKnown && !run->testProcessRecorded)) return UMI_STATUS_PARSE_ERROR;
    }
    if (run->closed) {
        if (run->afterSource[0] == '\0') run->closeBlockers |= UMI_QUALIFICATION_SOURCE_UNOBSERVED;
        else if (strcmp(run->inventory, run->afterInventory) != 0) run->closeBlockers |= UMI_QUALIFICATION_CONTEXT_CHANGED;
        else if (strcmp(run->source, run->afterSource) != 0) run->closeBlockers |= UMI_QUALIFICATION_SOURCE_CHANGED;
        if (run->afterToolchain[0] == '\0') run->closeBlockers |= UMI_QUALIFICATION_TOOLCHAIN_UNOBSERVED;
        else if (strcmp(run->toolchain, run->afterToolchain) != 0) run->closeBlockers |= UMI_QUALIFICATION_TOOLCHAIN_CHANGED;
    }
    UmiStatus status = UmiQualificationPlanFingerprint(run);
    if (status != UMI_STATUS_OK) return status;
    if (strcmp(expectedPlan, run->identity.plan) != 0) return UMI_STATUS_PARSE_ERROR;
    UmiReleaseQualificationSummary summary;
    status = UmiReleaseQualificationDescribe(run, &summary);
    if (status != UMI_STATUS_OK) return status;
    uint64_t actual[] = {summary.targets, summary.tests, summary.recorded, summary.missing,
        summary.passed, summary.failed, summary.skipped, summary.notRun, summary.cancelled, summary.timedOut,
        summary.disabled, summary.blockers, summary.closed ? 1U : 0U, summary.buildRecorded ? 1U : 0U,
        summary.readyForOwnerReview ? 1U : 0U, summary.testProcessRecorded ? 1U : 0U};
    for (size_t i = 0U; i < 16U; ++i) if (expected[i] != actual[i]) return UMI_STATUS_PARSE_ERROR;
    return UMI_STATUS_OK;
}
/* Preserve the earlier display-only reconstruction for review. The shared
 * private parser now checks the same invariants for display and retry planning;
 * neither public path can resume or mutate a saved execution. */
#if 0
UmiStatus UmiQualificationArchiveParse(const char *bytes, size_t length,
    UmiQualificationView **outView, UmiQualificationArchiveReceipt *outReceipt)
{
    if (bytes == NULL || outView == NULL || *outView != NULL || outReceipt == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (length >= UMI_QUALIFICATION_REPORT_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (length == 0U || memchr(bytes, '\0', length) != NULL) return UMI_STATUS_PARSE_ERROR;
    UmiReleaseQualification *run = calloc(1U, sizeof(*run));
    if (run == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiQualificationReader reader = {.next = bytes, .end = bytes + length};
    uint64_t summary[16] = {0}; char expectedPlan[65] = "";
    UmiStatus status = Prefix(&reader, run, summary, expectedPlan);
    if (status == UMI_STATUS_OK) status = Items(&reader, run);
    if (status == UMI_STATUS_OK) status = Validate(run, summary, expectedPlan);
    UmiQualificationArchiveReceipt receipt = {.identity = run->identity, .bytes = length};
    UmiQualificationView *view = NULL;
    if (status == UMI_STATUS_OK) status = UmiSha256Buffer(bytes, length, receipt.reportSha256);
    if (status == UMI_STATUS_OK) status = UmiQualificationViewCapture(run, &view);
    if (status == UMI_STATUS_OK) {
        UmiQualificationViewMarkSaved(view); *outView = view; *outReceipt = receipt;
    }
    UmiReleaseQualificationDestroy(run); return status;
}
#endif

UmiStatus UmiQualificationParseRecord(const char *bytes, size_t length,
    UmiReleaseQualification **outRun)
{
    if (bytes == NULL || outRun == NULL || *outRun != NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (length >= UMI_QUALIFICATION_REPORT_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (length == 0U || memchr(bytes, '\0', length) != NULL) return UMI_STATUS_PARSE_ERROR;
    UmiReleaseQualification *run = calloc(1U, sizeof(*run));
    if (run == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiQualificationReader reader = {.next = bytes, .end = bytes + length};
    uint64_t summary[16] = {0};
    char expectedPlan[65] = "";
    UmiStatus status = Prefix(&reader, run, summary, expectedPlan);
    if (status == UMI_STATUS_OK) status = Items(&reader, run);
    if (status == UMI_STATUS_OK) status = Validate(run, summary, expectedPlan);
    if (status == UMI_STATUS_OK) *outRun = run;
    else UmiReleaseQualificationDestroy(run);
    return status;
}

UmiStatus UmiQualificationArchiveParse(const char *bytes, size_t length,
    UmiQualificationView **outView, UmiQualificationArchiveReceipt *outReceipt)
{
    if (outView == NULL || *outView != NULL || outReceipt == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiReleaseQualification *run = NULL;
    UmiStatus status = UmiQualificationParseRecord(bytes, length, &run);
    if (status != UMI_STATUS_OK) return status;
    UmiQualificationArchiveReceipt receipt = {.identity = run->identity, .bytes = length};
    UmiQualificationView *view = NULL;
    status = UmiSha256Buffer(bytes, length, receipt.reportSha256);
    if (status == UMI_STATUS_OK) status = UmiQualificationViewCapture(run, &view);
    if (status == UMI_STATUS_OK) {
        UmiQualificationViewMarkSaved(view);
        *outView = view;
        *outReceipt = receipt;
    }
    UmiReleaseQualificationDestroy(run);
    return status;
}
