/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/qualification_ctest.c
 * Purpose: Reuse strict JUnit parsing and atomic exact-name result recording.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/distribution/runtime/qualification_ctest.h"
#include "umicom/testing/ctest_report.h"
#include "umicom/base/sha256.h"
#include <stdlib.h>
#include <string.h>

static UmiStatus Outcome(const UmiCtestReportEntry *entry, UmiReleaseQualificationResult *result)
{
    result->durationMs = entry->result.duration_ms;
    result->exitCodeKnown = false;
    result->exitCode = 0;
    result->status = UMI_STATUS_OK;
    if (entry->disabled) {
        result->outcome = UMI_QUALIFICATION_DISABLED;
        return UMI_STATUS_OK;
    }
    switch (entry->result.state) {
    case UMI_TEST_STATE_PASSED: result->outcome = UMI_QUALIFICATION_PASSED; break;
    case UMI_TEST_STATE_FAILED:
        result->outcome = UMI_QUALIFICATION_FAILED; result->status = UMI_STATUS_IO_ERROR; break;
    case UMI_TEST_STATE_SKIPPED: result->outcome = UMI_QUALIFICATION_SKIPPED; break;
    case UMI_TEST_STATE_NOT_RUN:
        result->outcome = UMI_QUALIFICATION_NOT_RUN; result->status = UMI_STATUS_UNAVAILABLE; break;
    case UMI_TEST_STATE_CANCELLED:
        result->outcome = UMI_QUALIFICATION_CANCELLED; result->status = UMI_STATUS_CANCELLED; break;
    case UMI_TEST_STATE_TIMED_OUT:
        result->outcome = UMI_QUALIFICATION_TIMED_OUT; result->status = UMI_STATUS_TIMEOUT; break;
    case UMI_TEST_STATE_RUNNING: return UMI_STATUS_INVALID_STATE;
    default: return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}
UmiStatus UmiReleaseQualificationImportCtest(UmiReleaseQualification *run,
    const UmiReleaseQualificationIdentity *identity, const char *xml, size_t length,
    const UmiReleaseQualificationResult *testProcess)
{
    if (run == NULL || identity == NULL || xml == NULL || testProcess == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiReleaseQualificationIdentity expected;
    UmiStatus status = UmiReleaseQualificationGetIdentity(run, &expected);
    if (status != UMI_STATUS_OK) return status;
    /* Fixed-width identity comparison also rejects unterminated caller fields
     * before parsing untrusted input. No string read can cross either array. */
    if (memcmp(expected.execution, identity->execution, sizeof(expected.execution)) != 0 ||
        memcmp(expected.plan, identity->plan, sizeof(expected.plan)) != 0) return UMI_STATUS_INVALID_STATE;
    UmiReleaseQualificationSummary selection;
    status = UmiReleaseQualificationDescribe(run, &selection);
    if (status != UMI_STATUS_OK) return status;
    if (selection.closed || selection.executionStatus != UMI_STATUS_OK || !selection.buildRecorded)
        return UMI_STATUS_INVALID_STATE;
    UmiCtestReport *report = NULL;
    status = UmiCtestReportImport(xml, length, &report);
    UmiCtestReportSummary summary = {0};
    if (status == UMI_STATUS_OK) status = UmiCtestReportReadSummary(report, &summary);
    if (status == UMI_STATUS_OK && summary.tests == 0U) status = UMI_STATUS_UNAVAILABLE;
    if (status == UMI_STATUS_OK && summary.tests != selection.tests) status = UMI_STATUS_INVALID_STATE;
    UmiReleaseQualificationNamedResult *results = NULL;
    if (status == UMI_STATUS_OK) {
        results = calloc(summary.tests, sizeof(*results));
        if (results == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
    }
    char digest[65];
    if (status == UMI_STATUS_OK) status = UmiSha256Buffer(xml, length, digest);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < summary.tests; ++i) {
        const UmiCtestReportEntry *entry = UmiCtestReportEntryAt(report, i);
        if (entry == NULL) { status = UMI_STATUS_INVALID_STATE; break; }
        results[i].name = entry->name;
        status = Outcome(entry, &results[i].result);
        memcpy(results[i].result.logSha256, digest, sizeof(digest));
    }
    /* The core validates every exact name and outcome before it writes any of
     * them. Parser success alone says nothing about scope, freshness or process success. */
    if (status == UMI_STATUS_OK)
        status = UmiReleaseQualificationRecordTests(run, identity, results, summary.tests, testProcess);
    free(results);
    UmiCtestReportDestroy(report);
    return status;
}
