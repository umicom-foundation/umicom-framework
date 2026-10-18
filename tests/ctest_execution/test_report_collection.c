/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ctest_execution/test_report_collection.c
 * PURPOSE: Verify complete saved report ownership and distinct test outcomes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/testing/ctest_report.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "Line %d: %s\n", __LINE__, #condition); return 1; } } while (0)

static const char mixed[] =
    "<testsuite tests='6' failures='2' disabled='1' skipped='2'>"
    "<testcase name='notes.pass' status='run' time='1.2349'/>"
    "<testcase name='notes.failure' status='fail' time='0.2'><failure message='Failed'/></testcase>"
    "<testcase name='notes.timeout' status='fail' time='0.3'><failure message='Timeout'/></testcase>"
    "<testcase name='notes.skip' status='notrun' time='0'><skipped message='SKIP_RETURN_CODE=77'/></testcase>"
    "<testcase name='notes.disabled' status='disabled' time='0'/>"
    "<testcase name='notes.missing' status='notrun' time='0'><skipped message='Unable to find executable'/></testcase>"
    "</testsuite>";

static int Rejected(const char *xml, size_t size, UmiStatus expected)
{
    UmiCtestReport *report = (UmiCtestReport *)(uintptr_t)1U;
    UmiStatus status = UmiCtestReportImport(xml, size, &report);
    CHECK(status == expected);
    CHECK(report == NULL);
    return 0;
}

static int VerifyMixed(int legacy)
{
    UmiCtestReport *report = NULL;
    CHECK(UmiCtestReportImport(mixed, sizeof mixed - 1U, &report) == UMI_STATUS_OK);
    UmiCtestReportSummary summary;
    CHECK(UmiCtestReportReadSummary(report, &summary) == UMI_STATUS_OK);
    CHECK(summary.tests == 6U && summary.passed == 1U && summary.failed == 1U);
    CHECK(summary.timed_out == 1U && summary.disabled == 1U && summary.skipped == 1U);
    CHECK(summary.not_run == 1U && summary.measured_test_ms == 1734U);
    CHECK(UmiCtestReportRequirePassed(report) == UMI_STATUS_INVALID_STATE);
    CHECK(UmiCtestReportEntryAt(report, 6U) == NULL);
    for (size_t index = 0U; index < summary.tests; ++index) {
        const UmiCtestReportEntry *entry = UmiCtestReportEntryAt(report, index);
        CHECK(entry != NULL && entry->result.testcase_count == 6U);
        CHECK(entry->disabled == (index == 4U));
        if (legacy) {
            UmiCtestReportSnapshot selected;
            CHECK(UmiCtestParseReport(mixed, sizeof mixed - 1U, entry->name, &selected) == UMI_STATUS_OK);
            CHECK(selected.state == entry->result.state);
            CHECK(selected.duration_ms == entry->result.duration_ms);
            CHECK(strcmp(selected.message, entry->result.message) == 0);
        }
    }
    UmiCtestReportDestroy(report);
    return 0;
}

static int VerifyPassed(void)
{
    const char *xml = "<testsuite tests='1' failures='0' disabled='0' skipped='0'>"
        "<testcase name='notes' status='run' time='1e-3'/></testsuite>";
    UmiCtestReport *report = NULL;
    CHECK(UmiCtestReportImport(xml, strlen(xml), &report) == UMI_STATUS_OK);
    CHECK(UmiCtestReportRequirePassed(report) == UMI_STATUS_OK);
    CHECK(UmiCtestReportEntryAt(report, 0U)->result.duration_ms == 1U);
    UmiCtestReportDestroy(report);
    return 0;
}

static int VerifyEmpty(void)
{
    const char *xml = "<testsuite tests='0'/>";
    UmiCtestReport *report = NULL;
    CHECK(UmiCtestReportImport(xml, strlen(xml), &report) == UMI_STATUS_OK);
    UmiCtestReportSummary summary;
    CHECK(UmiCtestReportReadSummary(report, &summary) == UMI_STATUS_OK && summary.tests == 0U);
    CHECK(UmiCtestReportRequirePassed(report) == UMI_STATUS_INVALID_STATE);
    CHECK(UmiCtestReportEntryAt(report, 0U) == NULL);
    UmiCtestReportDestroy(report);
    return 0;
}

static int VerifyTruncated(void)
{
    for (size_t length = 1U; length < sizeof mixed - 1U; ++length)
        CHECK(Rejected(mixed, length, UMI_STATUS_PARSE_ERROR) == 0);
    return 0;
}

static int VerifyTotals(void)
{
    const char *bad[] = {
        "<testsuite tests='1'/>",
        "<testsuite tests='0' failures='1'/>",
        "<testsuite tests='0' disabled='1'/>",
        "<testsuite tests='0' skipped='1'/>",
        "<testsuite tests='0' failures='many'/>",
        "<testsuite tests='1' skipped='1'><testcase name='off' status='disabled' time='0'/></testsuite>",
        "<testsuite tests='1' failures='0'><testcase name='late' status='fail' time='0'><failure message='Timeout'/></testcase></testsuite>"
    };
    for (size_t index = 0U; index < sizeof bad / sizeof bad[0]; ++index)
        CHECK(Rejected(bad[index], strlen(bad[index]), UMI_STATUS_PARSE_ERROR) == 0);
    return 0;
}

static int VerifyDuplicate(void)
{
    const char *xml = "<testsuite tests='2'>"
        "<testcase name='notes' status='run' time='0'/>"
        "<testcase name='n&#111;tes' status='run' time='0'/></testsuite>";
    CHECK(Rejected(xml, strlen(xml), UMI_STATUS_PARSE_ERROR) == 0);
    return 0;
}

static int VerifyEntities(void)
{
    const char *xml = "<?xml version='1.0' encoding='UTF-8'?><testsuite tests='1'>"
        "<!-- <testcase name='invented'/> -->"
        "<testcase name='caf&#233; &amp; save' status='fail' time='0'>"
        "<failure message='Line&#10;two'/><system-out><![CDATA[<testsuite tests='0'/>]]>"
        "</system-out></testcase></testsuite>";
    UmiCtestReport *report = NULL;
    CHECK(UmiCtestReportImport(xml, strlen(xml), &report) == UMI_STATUS_OK);
    const UmiCtestReportEntry *entry = UmiCtestReportEntryAt(report, 0U);
    CHECK(strcmp(entry->name, "caf\xC3\xA9 & save") == 0);
    CHECK(strcmp(entry->result.message, "Line\ntwo") == 0);
    UmiCtestReportDestroy(report);
    return 0;
}

static int VerifyLarge(void)
{
    const size_t count = 25018U, capacity = count * 96U + 100U;
    char *xml = malloc(capacity);
    CHECK(xml != NULL);
    int written = snprintf(xml, capacity, "<testsuite tests='%zu'>", count);
    CHECK(written > 0);
    size_t used = (size_t)written;
    for (size_t index = 0U; index < count; ++index) {
        written = snprintf(xml + used, capacity - used,
            "<testcase name='case.%zu' status='run' time='0.001'/>", index);
        CHECK(written > 0 && (size_t)written < capacity - used);
        used += (size_t)written;
    }
    const char closing[] = "</testsuite>";
    CHECK(sizeof closing < capacity - used);
    memcpy(xml + used, closing, sizeof closing);
    used += sizeof closing - 1U;
    UmiCtestReport *report = NULL;
    CHECK(UmiCtestReportImport(xml, used, &report) == UMI_STATUS_OK);
    free(xml);
    UmiCtestReportSummary summary;
    CHECK(UmiCtestReportReadSummary(report, &summary) == UMI_STATUS_OK);
    CHECK(summary.tests == count && summary.passed == count && summary.measured_test_ms == count);
    CHECK(strcmp(UmiCtestReportEntryAt(report, count - 1U)->name, "case.25017") == 0);
    CHECK(UmiCtestReportRequirePassed(report) == UMI_STATUS_OK);
    UmiCtestReportDestroy(report);
    return 0;
}

static int VerifyBounds(void)
{
    CHECK(Rejected("x", UMI_CTEST_COLLECTION_MAX_BYTES + 1U, UMI_STATUS_CAPACITY_EXCEEDED) == 0);
    const char *xml = "<testsuite tests='100001'/>";
    CHECK(Rejected(xml, strlen(xml), UMI_STATUS_CAPACITY_EXCEEDED) == 0);
    char longXml[700];
    char name[UMI_TEST_NAME_CAPACITY + 1U];
    memset(name, 'n', sizeof name - 1U);
    name[sizeof name - 1U] = '\0';
    int written = snprintf(longXml, sizeof longXml,
        "<testsuite tests='1'><testcase name='%s' status='run' time='0'/></testsuite>", name);
    CHECK(written > 0 && (size_t)written < sizeof longXml);
    CHECK(Rejected(longXml, (size_t)written, UMI_STATUS_CAPACITY_EXCEEDED) == 0);
    return 0;
}

static int VerifyDurationOverflow(void)
{
    const char *xml = "<testsuite tests='2'>"
        "<testcase name='one' status='run' time='18446744073709551.615'/>"
        "<testcase name='two' status='run' time='0.001'/></testsuite>";
    CHECK(Rejected(xml, strlen(xml), UMI_STATUS_CAPACITY_EXCEEDED) == 0);
    return 0;
}

static int VerifyHostile(void)
{
    const char *bad[] = {
        "<!DOCTYPE testsuite [<!ENTITY secret SYSTEM 'file:///private'>]><testsuite tests='0'/>",
        "<testsuite tests='1'><testcase name='n' status='unknown' time='0'/></testsuite>",
        "<testsuite tests='1'><testcase name='n' status='run' time='0'><failure message='Failed'/></testcase></testsuite>",
        "<testsuite tests='1'><testcase name='n' status='run' time='0'/>",
        "<testsuite tests='0'/><testsuite tests='0'/>",
        "<testsuite tests='1'><testcase name='\xC0\xAF' status='run' time='0'/></testsuite>",
        "<?xml version='1.0' encoding='UTF-16'?><testsuite tests='0'/>"
    };
    for (size_t index = 0U; index < sizeof bad / sizeof bad[0]; ++index)
        CHECK(Rejected(bad[index], strlen(bad[index]), UMI_STATUS_PARSE_ERROR) == 0);
    CHECK(Rejected("<testsuite\0 tests='0'/>", 22U, UMI_STATUS_PARSE_ERROR) == 0);
    return 0;
}

static int VerifyOwned(void)
{
    char *input = malloc(sizeof mixed);
    CHECK(input != NULL);
    memcpy(input, mixed, sizeof mixed);
    UmiCtestReport *report = NULL;
    CHECK(UmiCtestReportImport(input, sizeof mixed - 1U, &report) == UMI_STATUS_OK);
    memset(input, 'x', sizeof mixed);
    free(input);
    CHECK(strcmp(UmiCtestReportEntryAt(report, 0U)->name, "notes.pass") == 0);
    CHECK(strcmp(UmiCtestReportEntryAt(report, 5U)->result.message, "Unable to find executable") == 0);
    UmiCtestReportDestroy(report);
    return 0;
}

static int VerifyInvalid(void)
{
    CHECK(Rejected(NULL, 1U, UMI_STATUS_INVALID_ARGUMENT) == 0);
    CHECK(Rejected("", 0U, UMI_STATUS_INVALID_ARGUMENT) == 0);
    CHECK(UmiCtestReportImport(mixed, sizeof mixed - 1U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiCtestReportEntryAt(NULL, 0U) == NULL);
    CHECK(UmiCtestReportRequirePassed(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    UmiCtestReportSummary summary = {0};
    summary.tests = 17U;
    CHECK(UmiCtestReportReadSummary(NULL, &summary) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(summary.tests == 17U);
    UmiCtestReportDestroy(NULL);
    return 0;
}

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    if (strcmp(argv[1], "mixed") == 0) return VerifyMixed(0);
    if (strcmp(argv[1], "legacy") == 0) return VerifyMixed(1);
    if (strcmp(argv[1], "all-passed") == 0) return VerifyPassed();
    if (strcmp(argv[1], "empty") == 0) return VerifyEmpty();
    if (strcmp(argv[1], "truncated") == 0) return VerifyTruncated();
    if (strcmp(argv[1], "totals") == 0) return VerifyTotals();
    if (strcmp(argv[1], "duplicate") == 0) return VerifyDuplicate();
    if (strcmp(argv[1], "entities") == 0) return VerifyEntities();
    if (strcmp(argv[1], "large") == 0) return VerifyLarge();
    if (strcmp(argv[1], "bounds") == 0) return VerifyBounds();
    if (strcmp(argv[1], "duration-overflow") == 0) return VerifyDurationOverflow();
    if (strcmp(argv[1], "hostile") == 0) return VerifyHostile();
    if (strcmp(argv[1], "owned") == 0) return VerifyOwned();
    if (strcmp(argv[1], "invalid") == 0) return VerifyInvalid();
    fprintf(stderr, "Unknown scenario: %s\n", argv[1]);
    return 1;
}
