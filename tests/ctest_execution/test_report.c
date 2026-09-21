/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ctest_execution/test_report.c
 *
 * PURPOSE:
 *   Check exact selections and CTest report interpretation without launching
 *   a child process. Failed parses must not publish partial success records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/testing/ctest_adapter.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned checks = 0U;
static unsigned failures = 0U;
#define CHECK(condition) do { ++checks; if (!(condition)) { \
    ++failures; fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
} } while (0)

static void Expect(const char *label, const char *xml, const char *name,
    UmiStatus expectedStatus, UmiTestState expectedState, uint64_t duration,
    size_t cases)
{
    UmiCtestReportSnapshot snapshot;
    memset(&snapshot, 0xA5, sizeof snapshot);
    UmiCtestReportSnapshot before = snapshot;
    UmiStatus status = UmiCtestParseReport(xml, strlen(xml), name, &snapshot);
    if (status != expectedStatus) fprintf(stderr, "%s: status %d, expected %d\n",
        label, (int)status, (int)expectedStatus);
    CHECK(status == expectedStatus);
    if (status == UMI_STATUS_OK) {
        if (snapshot.duration_ms != duration) fprintf(stderr,
            "%s: duration %" PRIu64 ", expected %" PRIu64 "\n",
            label, snapshot.duration_ms, duration);
        CHECK(snapshot.state == expectedState);
        CHECK(snapshot.duration_ms == duration);
        CHECK(snapshot.testcase_count == cases);
    } else CHECK(memcmp(&snapshot, &before, sizeof snapshot) == 0);
}

#define ONE(body) "<testsuite tests=\"1\">" body "</testsuite>"
#define PASS "<testcase name=\"notes.save[1]+\" time=\"0.125\" status=\"run\"><properties/><system-out/></testcase>"
#define SIMPLE "<testcase name=\"notes\" time=\"0\" status=\"run\"/>"
#define BAD(label, xml) Expect(label, xml, "notes", UMI_STATUS_PARSE_ERROR, UMI_TEST_STATE_NOT_RUN, 0U, 0U)

int main(void)
{
    Expect("passed", ONE(PASS), "notes.save[1]+", UMI_STATUS_OK,
        UMI_TEST_STATE_PASSED, 125U, 1U);
    Expect("failed", ONE("<testcase name='notes' time='1.234' status='fail'><failure message='Failed'/></testcase>"),
        "notes", UMI_STATUS_OK, UMI_TEST_STATE_FAILED, 1234U, 1U);
    Expect("timeout", ONE("<testcase name='notes' time='1' status='fail'><failure message='Timeout'/></testcase>"),
        "notes", UMI_STATUS_OK, UMI_TEST_STATE_TIMED_OUT, 1000U, 1U);
    Expect("skip return code", ONE("<testcase name='notes' time='0' status='notrun'><skipped message='SKIP_RETURN_CODE=77'/></testcase>"),
        "notes", UMI_STATUS_OK, UMI_TEST_STATE_SKIPPED, 0U, 1U);
    Expect("skip expression", ONE("<testcase name='notes' time='0' status='notrun'><skipped message='SKIP_REGULAR_EXPRESSION_MATCHED'/></testcase>"),
        "notes", UMI_STATUS_OK, UMI_TEST_STATE_SKIPPED, 0U, 1U);
    Expect("disabled", ONE("<testcase name='notes' time='0' status='disabled'><system-out>Disabled</system-out></testcase>"),
        "notes", UMI_STATUS_OK, UMI_TEST_STATE_SKIPPED, 0U, 1U);
    Expect("missing executable", ONE("<testcase name='notes' time='0' status='notrun'><skipped message='Unable to find executable'/></testcase>"),
        "notes", UMI_STATUS_OK, UMI_TEST_STATE_NOT_RUN, 0U, 1U);
    Expect("fixture dependency", ONE("<testcase name='notes' time='0' status='notrun'><skipped message='Failed test dependencies: setup'/></testcase>"),
        "notes", UMI_STATUS_OK, UMI_TEST_STATE_NOT_RUN, 0U, 1U);
    Expect("fixture included", "<testsuite tests='2'>" SIMPLE PASS "</testsuite>",
        "notes.save[1]+", UMI_STATUS_OK, UMI_TEST_STATE_PASSED, 125U, 2U);
    Expect("name is literal", ONE(PASS), "notes.save1", UMI_STATUS_NOT_FOUND,
        UMI_TEST_STATE_NOT_RUN, 0U, 0U);
    Expect("no tests", "<testsuite tests='0'/>", "notes", UMI_STATUS_NOT_FOUND,
        UMI_TEST_STATE_NOT_RUN, 0U, 0U);
    Expect("entities", ONE("<testcase name='notes&amp;&quot;&apos;&lt;&gt;' status='run' time='0'/>"),
        "notes&\"'<>", UMI_STATUS_OK, UMI_TEST_STATE_PASSED, 0U, 1U);
    Expect("numeric unicode", ONE("<testcase name='notes&#x1F642;&#233;' status='run' time='0'/>"),
        "notes\xF0\x9F\x99\x82\xC3\xA9", UMI_STATUS_OK, UMI_TEST_STATE_PASSED, 0U, 1U);
    Expect("raw unicode", ONE("<testcase name='notes\xC3\xA9' status='run' time='0'/>"),
        "notes\xC3\xA9", UMI_STATUS_OK, UMI_TEST_STATE_PASSED, 0U, 1U);
    Expect("comments", "<!-- <testcase name='notes' status='fail'/> --><testsuite tests='1'>" SIMPLE "</testsuite><!-- retained -->",
        "notes", UMI_STATUS_OK, UMI_TEST_STATE_PASSED, 0U, 1U);
    Expect("CDATA cannot inject", ONE("<testcase name='notes' time='0' status='run'><system-out><![CDATA[<failure message='Failed'/><testcase name='notes'/>]]></system-out></testcase>"),
        "notes", UMI_STATUS_OK, UMI_TEST_STATE_PASSED, 0U, 1U);
    Expect("escaped output cannot inject", ONE("<testcase name='notes' time='0' status='run'><system-out>&lt;failure message=&quot;Failed&quot;/&gt;</system-out></testcase>"),
        "notes", UMI_STATUS_OK, UMI_TEST_STATE_PASSED, 0U, 1U);
    Expect("declaration", "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" ONE(SIMPLE),
        "notes", UMI_STATUS_OK, UMI_TEST_STATE_PASSED, 0U, 1U);
    Expect("BOM", "\xEF\xBB\xBF" ONE(SIMPLE), "notes", UMI_STATUS_OK,
        UMI_TEST_STATE_PASSED, 0U, 1U);

    BAD("duplicate match", "<testsuite tests='2'>" SIMPLE SIMPLE "</testsuite>");
    BAD("wrong count", "<testsuite tests='2'>" SIMPLE "</testsuite>");
    BAD("negative count", "<testsuite tests='-1'>" SIMPLE "</testsuite>");
    BAD("overflowing count", "<testsuite tests='999999999999999999999999999999'>" SIMPLE "</testsuite>");
    BAD("missing count", "<testsuite>" SIMPLE "</testsuite>");
    BAD("missing name", ONE("<testcase time='0' status='run'/>"));
    BAD("missing time", ONE("<testcase name='notes' status='run'/>"));
    BAD("missing state", ONE("<testcase name='notes' time='0'/>"));
    BAD("unknown state", ONE("<testcase name='notes' time='0' status='maybe'/>"));
    BAD("contradictory pass", ONE("<testcase name='notes' time='0' status='run'><failure message='Failed'/></testcase>"));
    BAD("contradictory skip", ONE("<testcase name='notes' time='0' status='run'><skipped message='SKIP_RETURN_CODE=1'/></testcase>"));
    BAD("missing failure", ONE("<testcase name='notes' time='0' status='fail'/>"));
    BAD("two failures", ONE("<testcase name='notes' time='0' status='fail'><failure message='Failed'/><failure message='Failed'/></testcase>"));
    BAD("duplicate attribute", ONE("<testcase name='notes' name='other' time='0' status='run'/>"));
    BAD("no attribute space", ONE("<testcase name='notes'time='0' status='run'/>"));
    BAD("missing quote", ONE("<testcase name='notes time='0' status='run'/>"));
    BAD("raw angle in attribute", ONE("<testcase name='notes<' time='0' status='run'/>"));
    BAD("unknown entity", ONE("<testcase name='notes&bogus;' time='0' status='run'/>"));
    BAD("invalid scalar", ONE("<testcase name='notes&#0;' time='0' status='run'/>"));
    BAD("surrogate entity", ONE("<testcase name='notes&#xD800;' time='0' status='run'/>"));
    BAD("out of range entity", ONE("<testcase name='notes&#x110000;' time='0' status='run'/>"));
    BAD("bad UTF8", ONE("<testcase name='notes\xC0\x80' time='0' status='run'/>"));
    BAD("missing close", "<testsuite tests='1'>" SIMPLE);
    BAD("wrong closing tag", "<testsuite tests='1'>" SIMPLE "</testcase>");
    BAD("second root", ONE(SIMPLE) ONE(SIMPLE));
    BAD("nested testcase", ONE("<testcase name='notes' time='0' status='run'>" SIMPLE "</testcase>"));
    BAD("unknown tag", ONE(SIMPLE "<unexpected/>"));
    BAD("doctype", "<!DOCTYPE testsuite SYSTEM 'file:///etc/passwd'>" ONE(SIMPLE));
    BAD("internal entity declaration", "<!DOCTYPE testsuite [<!ENTITY x 'notes'>]>" ONE(SIMPLE));
    BAD("processing instruction", "<?external any?>" ONE(SIMPLE));
    BAD("invalid declaration", "<?xml garbage?>" ONE(SIMPLE));
    BAD("false encoding", "<?xml version='1.0' encoding='UTF-16'?>" ONE(SIMPLE));
    BAD("invalid comment", "<!-- illegal -- comment -->" ONE(SIMPLE));
    BAD("invalid CDATA", ONE("<testcase name='notes' time='0' status='run'><system-out><![CDATA[\x01]]></system-out></testcase>"));
    BAD("raw CDATA end", ONE("<testcase name='notes' time='0' status='run'><system-out>]]></system-out></testcase>"));
    BAD("CDATA outside text", "<![CDATA[text]]>" ONE(SIMPLE));
    BAD("trailing text", ONE(SIMPLE) "unrelated");
    BAD("entity outside root", "&#32;" ONE(SIMPLE));
    BAD("negative time", ONE("<testcase name='notes' time='-1' status='run'/>"));
    BAD("infinite time", ONE("<testcase name='notes' time='inf' status='run'/>"));
    BAD("NaN time", ONE("<testcase name='notes' time='nan' status='run'/>"));
    BAD("decimal locale", ONE("<testcase name='notes' time='0,5' status='run'/>"));
    BAD("bad exponent", ONE("<testcase name='notes' time='1e' status='run'/>"));
    BAD("huge exponent", ONE("<testcase name='notes' time='1e999' status='run'/>"));

    const struct { const char *text; uint64_t expected; } timings[] = {
        {"0", 0U}, {"0.003", 3U}, {"0.001", 1U}, {"0.009", 9U},
        {"0.999999", 999U}, {"0.0009", 0U}, {"5e-3", 5U},
        {"2E+3", 2000000U}, {"0.125", 125U},
        {"18446744073709551.615", UINT64_MAX}
    };
    for (size_t index = 0U; index < sizeof timings / sizeof timings[0]; ++index) {
        char xml[256];
        snprintf(xml, sizeof xml, "<testsuite tests='1'><testcase name='notes' status='run' time='%s'/></testsuite>", timings[index].text);
        Expect(timings[index].text, xml, "notes", UMI_STATUS_OK,
            UMI_TEST_STATE_PASSED, timings[index].expected, 1U);
    }

    char pattern[UMI_CTEST_EXACT_PATTERN_CAPACITY];
    CHECK(UmiCtestEscapeName("notes.save[1]+", pattern, sizeof pattern) == UMI_STATUS_OK);
    CHECK(strcmp(pattern, "^notes\\.save\\[1\\]\\+$") == 0);
    CHECK(UmiCtestEscapeName(".^$[](){}*+?|\\", pattern, sizeof pattern) == UMI_STATUS_OK);
    CHECK(strcmp(pattern, "^\\.\\^\\$\\[\\]\\(\\)\\{\\}\\*\\+\\?\\|\\\\$") == 0);
    strcpy(pattern, "unchanged");
    CHECK(UmiCtestEscapeName("notes", pattern, 2U) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(strcmp(pattern, "unchanged") == 0);
    CHECK(UmiCtestEscapeName("", pattern, sizeof pattern) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiCtestEscapeName("notes\nother", pattern, sizeof pattern) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiCtestEscapeName("notes\xFF", pattern, sizeof pattern) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiCtestEscapeName(NULL, pattern, sizeof pattern) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiCtestEscapeName("notes", NULL, 1U) == UMI_STATUS_INVALID_ARGUMENT);
    char tooLong[UMI_TEST_NAME_CAPACITY + 1U];
    memset(tooLong, 'x', sizeof tooLong - 1U); tooLong[sizeof tooLong - 1U] = '\0';
    CHECK(UmiCtestEscapeName(tooLong, pattern, sizeof pattern) == UMI_STATUS_CAPACITY_EXCEEDED);
    UmiCtestReportSnapshot snapshot;
    CHECK(UmiCtestParseReport(NULL, 1U, "notes", &snapshot) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiCtestParseReport("x", 0U, "notes", &snapshot) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiCtestParseReport("x", UMI_CTEST_REPORT_MAX_BYTES + 1U, "notes", &snapshot) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(UmiCtestParseReport("x\0y", 3U, "notes", &snapshot) == UMI_STATUS_PARSE_ERROR);
    UmiTestResult result; memset(&result, 0, sizeof result);
    result.state = UMI_TEST_STATE_PASSED;
    CHECK(UmiCtestResultRequirePassed(&result) == UMI_STATUS_OK);
    result.state = UMI_TEST_STATE_SKIPPED;
    CHECK(UmiCtestResultRequirePassed(&result) == UMI_STATUS_INVALID_STATE);
    result.state = UMI_TEST_STATE_NOT_RUN;
    CHECK(UmiCtestResultRequirePassed(&result) == UMI_STATUS_INVALID_STATE);
    result.state = UMI_TEST_STATE_PASSED; result.exit_code = 1;
    CHECK(UmiCtestResultRequirePassed(&result) == UMI_STATUS_INVALID_STATE);
    result.status = UMI_STATUS_IO_ERROR;
    CHECK(UmiCtestResultRequirePassed(&result) == UMI_STATUS_IO_ERROR);
    CHECK(UmiCtestResultRequirePassed(NULL) == UMI_STATUS_INVALID_ARGUMENT);

    printf("CTest report checks: %u; failures: %u\n", checks, failures);
    return failures == 0U ? EXIT_SUCCESS : EXIT_FAILURE;
}
