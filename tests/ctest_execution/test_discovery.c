/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ctest_execution/test_discovery.c
 * PURPOSE: Verify transactional discovery, identities and complete-listing contracts.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/testing/discovery.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define REQUIRE(condition) do { if (!(condition)) { \
    fprintf(stderr, "Check failed at %s:%d: %s\n", __FILE__, __LINE__, #condition); \
    return 1; } } while (0)

static const char complete[] = "Test project /tmp/notes\n  Test #1: notes.save[1]+\n  Test #2: bank.review\n\nTotal Tests: 2\n";

int main(int argc, char **argv)
{
    REQUIRE(argc == 2);
    const char *scenario = argv[1];
    UmiTestSuite *suite = NULL;
    REQUIRE(umi_test_suite_create("retained", "Retained catalogue", &suite) == UMI_STATUS_OK);
    UmiTestCase kept;
    umi_test_case_init(&kept, "retained.test", "Retained test");
    REQUIRE(umi_test_suite_add(suite, &kept) == UMI_STATUS_OK);
    const UmiTestCase *borrowed = umi_test_suite_at(suite, 0U);
    size_t count = 777U;
    UmiStatus status = UMI_STATUS_OK;
    size_t expectedCount = 1U;
    if (strcmp(scenario, "complete") == 0) {
        status = UmiTestDiscoveryParseCtestComplete(complete, strlen(complete), "/tmp/notes", suite, &count);
        REQUIRE(status == UMI_STATUS_OK && count == 2U);
        REQUIRE(strcmp(umi_test_suite_at(suite, 1U)->name, "notes.save[1]+") == 0);
        REQUIRE(strcmp(umi_test_suite_at(suite, 2U)->working_directory, "/tmp/notes") == 0);
        REQUIRE(umi_test_suite_at(suite, 1U)->kind == UMI_TEST_KIND_EXTERNAL);
        expectedCount = 3U;
    } else if (strcmp(scenario, "legacy-fragment") == 0) {
        status = umi_test_discovery_parse_ctest("Test #4: trader.orders\n", "/tmp", suite, &count);
        REQUIRE(status == UMI_STATUS_OK && count == 1U); expectedCount = 2U;
    } else if (strcmp(scenario, "missing-summary") == 0) {
        const char *text = "Test #1: notes.save\n";
        status = UmiTestDiscoveryParseCtestComplete(text, strlen(text), "/tmp", suite, &count);
        REQUIRE(status == UMI_STATUS_PARSE_ERROR && count == 0U);
    } else if (strcmp(scenario, "empty") == 0) {
        const char *text = "Test project /tmp\n\nTotal Tests: 0\n";
        status = UmiTestDiscoveryParseCtestComplete(text, strlen(text), "/tmp", suite, &count);
        REQUIRE(status == UMI_STATUS_OK && count == 0U);
    } else if (strcmp(scenario, "wrong-total") == 0) {
        const char *text = "Test #1: notes.save\nTotal Tests: 2\n";
        status = UmiTestDiscoveryParseCtestComplete(text, strlen(text), "/tmp", suite, &count);
        REQUIRE(status == UMI_STATUS_PARSE_ERROR && count == 0U);
    } else if (strcmp(scenario, "duplicate-incoming") == 0) {
        const char *text = "Test #1: notes.save\nTest #2: notes.save\nTotal Tests: 2\n";
        status = UmiTestDiscoveryParseCtestComplete(text, strlen(text), "/tmp", suite, &count);
        REQUIRE(status == UMI_STATUS_ALREADY_EXISTS && count == 0U);
    } else if (strcmp(scenario, "duplicate-existing") == 0) {
        const char *text = "Test #1: notes.save\nTest #2: retained.test\nTotal Tests: 2\n";
        status = UmiTestDiscoveryParseCtestComplete(text, strlen(text), "/tmp", suite, &count);
        REQUIRE(status == UMI_STATUS_ALREADY_EXISTS && count == 0U);
    } else if (strcmp(scenario, "duplicate-summary") == 0) {
        const char *text = "Total Tests: 0\nTotal Tests: 0\n";
        status = UmiTestDiscoveryParseCtestComplete(text, strlen(text), "/tmp", suite, &count);
        REQUIRE(status == UMI_STATUS_PARSE_ERROR && count == 0U);
    } else if (strcmp(scenario, "row-after-summary") == 0) {
        const char *text = "Total Tests: 0\nTest #1: notes.save\n";
        status = UmiTestDiscoveryParseCtestComplete(text, strlen(text), "/tmp", suite, &count);
        REQUIRE(status == UMI_STATUS_PARSE_ERROR && count == 0U);
    } else if (strcmp(scenario, "number-overflow") == 0) {
        const char *text = "Test #99999999999999999999999999999999999: notes.save\nTotal Tests: 1\n";
        status = UmiTestDiscoveryParseCtestComplete(text, strlen(text), "/tmp", suite, &count);
        REQUIRE(status == UMI_STATUS_CAPACITY_EXCEEDED && count == 0U);
    } else if (strcmp(scenario, "malformed-row") == 0) {
        const char *text = "Test #1: notes.save\nTest #x: bank.payment\nTotal Tests: 1\n";
        status = UmiTestDiscoveryParseCtestComplete(text, strlen(text), "/tmp", suite, &count);
        REQUIRE(status == UMI_STATUS_PARSE_ERROR && count == 0U);
    } else if (strcmp(scenario, "duplicate-number") == 0) {
        const char *text = "Test #1: notes.save\nTest #1: bank.payment\nTotal Tests: 2\n";
        status = UmiTestDiscoveryParseCtestComplete(text, strlen(text), "/tmp", suite, &count);
        REQUIRE(status == UMI_STATUS_PARSE_ERROR && count == 0U);
    } else if (strcmp(scenario, "empty-name") == 0) {
        const char *text = "Test #1: \nTotal Tests: 1\n";
        status = UmiTestDiscoveryParseCtestComplete(text, strlen(text), "/tmp", suite, &count);
        REQUIRE(status == UMI_STATUS_PARSE_ERROR && count == 0U);
    } else if (strcmp(scenario, "long-name") == 0) {
        char text[600];
        strcpy(text, "Test #1: "); memset(text + 9, 'n', 256U);
        strcpy(text + 265, "\nTotal Tests: 1\n");
        status = UmiTestDiscoveryParseCtestComplete(text, strlen(text), "/tmp", suite, &count);
        REQUIRE(status == UMI_STATUS_CAPACITY_EXCEEDED && count == 0U);
    } else if (strcmp(scenario, "identity-limit") == 0) {
        char text[300];
        strcpy(text, "Test #1: "); memset(text + 9, 'i', UMI_TEST_ID_CAPACITY);
        strcpy(text + 9 + UMI_TEST_ID_CAPACITY, "\nTotal Tests: 1\n");
        status = UmiTestDiscoveryParseCtestComplete(text, strlen(text), "/tmp", suite, &count);
        REQUIRE(status == UMI_STATUS_CAPACITY_EXCEEDED && count == 0U);
    } else if (strcmp(scenario, "embedded-nul") == 0) {
        const char text[] = "Total Tests: 0\n\0Test #1: hidden\n";
        status = UmiTestDiscoveryParseCtestComplete(text, sizeof text - 1U, "/tmp", suite, &count);
        REQUIRE(status == UMI_STATUS_PARSE_ERROR && count == 0U);
    } else if (strcmp(scenario, "crlf-spaces") == 0) {
        const char *text = "\tTest    # 9:  notes save \r\n\r\nTotal Tests: 1\r\n";
        status = UmiTestDiscoveryParseCtestComplete(text, strlen(text), "/tmp", suite, &count);
        REQUIRE(status == UMI_STATUS_OK && count == 1U);
        REQUIRE(strcmp(umi_test_suite_at(suite, 1U)->name, " notes save ") == 0);
        expectedCount = 2U;
    } else if (strcmp(scenario, "unicode") == 0) {
        const char *text = "Test #1: notes.\xCE\xB1\nTotal Tests: 1\n";
        status = UmiTestDiscoveryParseCtestComplete(text, strlen(text), "/tmp", suite, &count);
        REQUIRE(status == UMI_STATUS_OK && count == 1U); expectedCount = 2U;
    } else if (strcmp(scenario, "ambiguous-disabled") == 0) {
        const char *text = "Test #1: notes.save (Disabled)\nTotal Tests: 1\n";
        status = UmiTestDiscoveryParseCtestComplete(text, strlen(text), "/tmp", suite, &count);
        REQUIRE(status == UMI_STATUS_UNAVAILABLE && count == 0U);
    } else if (strcmp(scenario, "legacy-disabled-literal") == 0) {
        status = umi_test_discovery_parse_ctest("Test #1: notes.save (Disabled)\n", "/tmp", suite, &count);
        REQUIRE(status == UMI_STATUS_OK && count == 1U); expectedCount = 2U;
        REQUIRE(strcmp(umi_test_suite_at(suite, 1U)->name, "notes.save (Disabled)") == 0);
    } else if (strcmp(scenario, "capacity-1025") == 0 || strcmp(scenario, "capacity-exact") == 0) {
        size_t wanted = strcmp(scenario, "capacity-exact") == 0 ? 1023U : 1025U;
        char *text = (char *)calloc(1U, 65536U); REQUIRE(text != NULL);
        size_t used = 0U;
        for (size_t i = 0U; i < wanted; ++i) {
            int written = snprintf(text + used, 65536U - used, "Test #%zu: notes.case.%zu\n", i + 1U, i);
            REQUIRE(written > 0 && (size_t)written < 65536U - used); used += (size_t)written;
        }
        int written = snprintf(text + used, 65536U - used, "Total Tests: %zu\n", wanted);
        REQUIRE(written > 0); used += (size_t)written;
        status = UmiTestDiscoveryParseCtestComplete(text, used, "/tmp", suite, &count);
        if (wanted == 1023U) { REQUIRE(status == UMI_STATUS_OK && count == wanted); expectedCount = 1024U; }
        else REQUIRE(status == UMI_STATUS_CAPACITY_EXCEEDED && count == 0U);
        free(text);
    } else if (strcmp(scenario, "invalid-input") == 0) {
        status = UmiTestDiscoveryParseCtestComplete(NULL, 3U, "/tmp", suite, &count);
        REQUIRE(status == UMI_STATUS_INVALID_ARGUMENT && count == 0U);
        count = 7U;
        status = UmiTestDiscoveryParseCtestComplete(complete, strlen(complete), "", suite, &count);
        REQUIRE(status == UMI_STATUS_INVALID_ARGUMENT && count == 0U);
        status = UmiTestDiscoveryParseCtestComplete(complete, 8U*1024U*1024U+1U, "/tmp", suite, &count);
        REQUIRE(status == UMI_STATUS_CAPACITY_EXCEEDED && count == 0U);
    } else if (strcmp(scenario, "long-build-root") == 0) {
        char path[UMI_TEST_COMMAND_CAPACITY + 1U]; memset(path, 'a', sizeof path - 1U); path[sizeof path - 1U] = '\0';
        status = UmiTestDiscoveryParseCtestComplete(complete, strlen(complete), path, suite, &count);
        REQUIRE(status == UMI_STATUS_CAPACITY_EXCEEDED && count == 0U);
    } else if (strcmp(scenario, "prefix-rejection") == 0) {
        const char *summaryEnd = strstr(complete, "Total Tests: 2") + strlen("Total Tests: 2");
        for (size_t n = 0U; n < (size_t)(summaryEnd - complete); ++n) {
            count = 99U;
            status = UmiTestDiscoveryParseCtestComplete(complete, n, "/tmp", suite, &count);
            REQUIRE(status != UMI_STATUS_OK && count == 0U);
            REQUIRE(umi_test_suite_count(suite) == 1U);
        }
    } else if (strcmp(scenario, "bulk-atomic") == 0) {
        UmiTestCase cases[2];
        umi_test_case_init(&cases[0], "new.first", "First");
        umi_test_case_init(&cases[1], "new.first", "Duplicate");
        REQUIRE(UmiTestSuiteAppendCases(suite, cases, 2U) == UMI_STATUS_ALREADY_EXISTS);
        REQUIRE(umi_test_suite_count(suite) == 1U);
        memset(cases[1].test_id, 'x', sizeof cases[1].test_id);
        REQUIRE(UmiTestSuiteAppendCases(suite, cases, 2U) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(umi_test_suite_count(suite) == 1U);
        REQUIRE(UmiTestSuiteAppendCases(suite, NULL, 0U) == UMI_STATUS_OK);
        REQUIRE(UmiTestSuiteAppendCases(suite, cases, (size_t)-1) == UMI_STATUS_CAPACITY_EXCEEDED);
        umi_test_case_init(&cases[1], "new.second", "Second");
        REQUIRE(UmiTestSuiteAppendCases(suite, cases, 2U) == UMI_STATUS_OK);
        expectedCount = 3U;
    } else { fprintf(stderr, "Unknown scenario: %s\n", scenario); return 2; }
    REQUIRE(umi_test_suite_count(suite) == expectedCount);
    REQUIRE(umi_test_suite_at(suite, 0U) == borrowed);
    REQUIRE(memcmp(umi_test_suite_at(suite, 0U), &kept, sizeof kept) == 0);
    umi_test_suite_destroy(suite);
    printf("discovery.%s passed\n", scenario);
    return 0;
}
