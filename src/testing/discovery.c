/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/discovery.c
 *
 * PURPOSE:
 *   Parse CTest list output into executable Framework test cases.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/testing/discovery.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* Bound input work independently of the number of records that fit the suite. */
#define CTEST_LISTING_MAX_BYTES (8U * 1024U * 1024U)

typedef struct CtestListingState {
    UmiTestCase *cases;
    size_t count;
    size_t expected;
    size_t numbers[UMI_TEST_SUITE_MAX_CASES];
    int hasSummary;
    int requireSummary;
} CtestListingState;

/* CTest's structural indentation is ASCII. Do not trim the test name itself:
 * leading/trailing spaces can be part of its literal identity. */
static const char *CtestListingSpace(const char *cursor, const char *end)
{
    while (cursor < end && (*cursor == ' ' || *cursor == '\t')) ++cursor;
    return cursor;
}

static int CtestListingPrefix(const char *cursor, const char *end,
    const char *prefix)
{
    size_t length = strlen(prefix);
    return (size_t)(end - cursor) >= length &&
        memcmp(cursor, prefix, length) == 0;
}

/* Parse decimal digits without scanf's integer-conversion overflow. */
static UmiStatus CtestListingNumber(const char **cursor, const char *end,
    size_t *outValue)
{
    const char *at = *cursor;
    if (at == end || *at < '0' || *at > '9') return UMI_STATUS_PARSE_ERROR;
    size_t value = 0U;
    while (at < end && *at >= '0' && *at <= '9') {
        size_t digit = (size_t)(*at - '0');
        if (value > (SIZE_MAX - digit) / 10U)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        value = value * 10U + digit;
        ++at;
    }
    *cursor = at; *outValue = value;
    return UMI_STATUS_OK;
}

static UmiStatus CtestListingLine(CtestListingState *state,
    const char *start, const char *end, const char *buildDirectory)
{
    if (end > start && end[-1] == '\r') --end;
    const char *cursor = CtestListingSpace(start, end);
    if (cursor == end) return UMI_STATUS_OK;
    if (state->hasSummary) return UMI_STATUS_PARSE_ERROR;
    if (CtestListingPrefix(cursor, end, "Total Tests:")) {
        cursor = CtestListingSpace(cursor + strlen("Total Tests:"), end);
        UmiStatus status = CtestListingNumber(&cursor, end, &state->expected);
        if (status != UMI_STATUS_OK) return status;
        if (CtestListingSpace(cursor, end) != end) return UMI_STATUS_PARSE_ERROR;
        state->hasSummary = 1;
        return UMI_STATUS_OK;
    }
    const char *marker = CtestListingPrefix(cursor, end, "Test")
        ? CtestListingSpace(cursor + 4U, end) : end;
    if (marker == end || *marker != '#') {
        /* Headers and missing-executable diagnostics precede the summary.
         * A misspelled count/test marker must not quietly become a header. */
        if (CtestListingPrefix(cursor, end, "Total Tests") ||
            CtestListingPrefix(cursor, end, "Test#"))
            return UMI_STATUS_PARSE_ERROR;
        return UMI_STATUS_OK;
    }
    if (marker == cursor + 4U) return UMI_STATUS_PARSE_ERROR;
    cursor = CtestListingSpace(marker + 1U, end);
    size_t number = 0U;
    UmiStatus status = CtestListingNumber(&cursor, end, &number);
    if (status != UMI_STATUS_OK) return status;
    if (number == 0U || cursor == end || *cursor++ != ':' ||
        cursor == end || *cursor++ != ' ' || cursor == end)
        return UMI_STATUS_PARSE_ERROR;
    size_t length = (size_t)(end - cursor);
    if (length >= UMI_TEST_ID_CAPACITY || length >= UMI_TEST_NAME_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t index = 0U; index < length; ++index) {
        unsigned char byte = (unsigned char)cursor[index];
        if (byte < 0x20U || byte == 0x7FU) return UMI_STATUS_PARSE_ERROR;
    }
    /* This marker is not escaped in CTest's human format. It could describe a
     * disabled test or belong to an enabled test's real name. Never guess.
     * The compatibility fragment parser retains its historical literal rule. */
    static const char disabledSuffix[] = " (Disabled)";
    if (state->requireSummary && length >= sizeof disabledSuffix - 1U &&
        memcmp(end - (sizeof disabledSuffix - 1U), disabledSuffix,
            sizeof disabledSuffix - 1U) == 0)
        return UMI_STATUS_UNAVAILABLE;
    if (state->count == UMI_TEST_SUITE_MAX_CASES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t index = 0U; index < state->count; ++index) {
        if (state->numbers[index] == number) return UMI_STATUS_PARSE_ERROR;
    }
    state->numbers[state->count] = number;
    UmiTestCase *item = &state->cases[state->count];
    char name[UMI_TEST_NAME_CAPACITY];
    memcpy(name, cursor, length); name[length] = '\0';
    umi_test_case_init(item, name, name);
    item->kind = UMI_TEST_KIND_EXTERNAL;
    item->timeout_ms = 0U;
    status = umi_test_case_set_command(item, "ctest", name, buildDirectory);
    if (status != UMI_STATUS_OK) return status;
    ++state->count;
    return UMI_STATUS_OK;
}

static UmiStatus CtestListingParse(const char *output, size_t length,
    const char *buildDirectory, UmiTestSuite *suite, size_t *outDiscovered,
    int requireSummary)
{
    if (outDiscovered != NULL) *outDiscovered = 0U;
    if (output == NULL || buildDirectory == NULL || suite == NULL ||
        buildDirectory[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    if (length > CTEST_LISTING_MAX_BYTES ||
        strlen(buildDirectory) >= UMI_TEST_COMMAND_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (memchr(output, '\0', length) != NULL) return UMI_STATUS_PARSE_ERROR;
    CtestListingState state = {0};
    state.requireSummary = requireSummary;
    state.cases = (UmiTestCase *)calloc(UMI_TEST_SUITE_MAX_CASES,
        sizeof *state.cases);
    if (state.cases == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UMI_STATUS_OK;
    const char *cursor = output, *limit = output + length;
    while (cursor < limit) {
        const char *end = (const char *)memchr(cursor, '\n',
            (size_t)(limit - cursor));
        if (end == NULL) end = limit;
        status = CtestListingLine(&state, cursor, end, buildDirectory);
        if (status != UMI_STATUS_OK) break;
        cursor = end < limit ? end + 1 : limit;
    }
    if (status == UMI_STATUS_OK &&
        ((requireSummary && !state.hasSummary) ||
         (state.hasSummary && state.expected != state.count)))
        status = UMI_STATUS_PARSE_ERROR;
    /* Only this operation publishes new records; failed parsing and failed
     * capacity/duplicate checks leave every previous suite record untouched. */
    if (status == UMI_STATUS_OK)
        status = UmiTestSuiteAppendCases(suite, state.cases, state.count);
    if (status == UMI_STATUS_OK && outDiscovered != NULL)
        *outDiscovered = state.count;
    free(state.cases);
    return status;
}

/*
 * Provide the test discovery parse ctest operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_discovery_parse_ctest(const char *output,
    const char *build_directory, UmiTestSuite *suite, size_t *out_discovered)
{
    /* Preserve compatibility for callers intentionally importing fragments.
     * Live discovery uses the complete-listing entry point below instead. */
    return CtestListingParse(output, output != NULL ? strlen(output) : 0U,
        build_directory, suite, out_discovered, 0);
}

UmiStatus UmiTestDiscoveryParseCtestComplete(const char *output, size_t length,
    const char *buildDirectory, UmiTestSuite *suite, size_t *outDiscovered)
{
    return CtestListingParse(output, length, buildDirectory, suite,
        outDiscovered, 1);
}
