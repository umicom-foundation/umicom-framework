/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_quick_open_query.c
 * PURPOSE: Check bounded source queries without file access or native widgets.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/quick_open_query.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr, "line %d: %s\n", __LINE__, #c); return 1; } } while (0)
static int Accepted(const char *input, const char *filter, size_t line, size_t column)
{
    UmiQuickOpenQuery query;
    memset(&query, 0xa5, sizeof query);
    CHECK(UmiQuickOpenQueryParse(input, strlen(input), &query) == UMI_STATUS_OK);
    CHECK(strcmp(query.filter, filter) == 0);
    CHECK(query.has_location == (line != 0U));
    CHECK(query.location.line == line && query.location.column == column);
    return 0;
}
static int Refused(const char *input, size_t bytes, UmiStatus expected)
{
    UmiQuickOpenQuery query, before;
    memset(&query, 0xa5, sizeof query); memcpy(&before, &query, sizeof query);
    CHECK(UmiQuickOpenQueryParse(input, bytes, &query) == expected);
    CHECK(memcmp(&query, &before, sizeof query) == 0);
    return 0;
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    if (strcmp(mode, "filters") == 0) {
        CHECK(Accepted("", "", 0U, 0U) == 0);
        CHECK(Accepted("main.c", "main.c", 0U, 0U) == 0);
        CHECK(Accepted(" src/my file.c ", " src/my file.c ", 0U, 0U) == 0);
        CHECK(Accepted("caf\xc3\xa9.c", "caf\xc3\xa9.c", 0U, 0U) == 0);
    } else if (strcmp(mode, "locations") == 0) {
        CHECK(Accepted("main.c:42", "main.c", 42U, 1U) == 0);
        CHECK(Accepted("src/main.c:42:5", "src/main.c", 42U, 5U) == 0);
        CHECK(Accepted("main.c: 007:002 ", "main.c", 7U, 2U) == 0);
        CHECK(Accepted("caf\xc3\xa9.c:2:3", "caf\xc3\xa9.c", 2U, 3U) == 0);
    } else if (strcmp(mode, "quoted") == 0) {
        CHECK(Accepted("\"notes:12\"", "notes:12", 0U, 0U) == 0);
        CHECK(Accepted("\"notes:12\":4:2", "notes:12", 4U, 2U) == 0);
        CHECK(Accepted("\"my notes.c\":4", "my notes.c", 4U, 1U) == 0);
    } else if (strcmp(mode, "paths") == 0) {
        CHECK(Accepted("C:/src/main.c", "C:/src/main.c", 0U, 0U) == 0);
        CHECK(Accepted("C:/src/main.c:8:3", "C:/src/main.c", 8U, 3U) == 0);
        CHECK(Accepted("C:\\src\\main.c:8", "C:\\src\\main.c", 8U, 1U) == 0);
        CHECK(Accepted("\\\\server\\share\\main.c:8", "\\\\server\\share\\main.c", 8U, 1U) == 0);
        CHECK(Accepted("/notes:old/main.c:8", "/notes:old/main.c", 8U, 1U) == 0);
        CHECK(Accepted("C:/", "C:/", 0U, 0U) == 0);
    } else if (strcmp(mode, "invalid") == 0) {
        const char *invalid[] = {":2", "main.c:", "main.c:0", "main.c:1:0", "main.c:-1",
            "main.c:1:2:3", "main.c:1:", "main.c:+2", "main.c:1 2", "main.c:abc",
            "\"\"", "\"main.c", "main\".c", "\"main.c\"tail", "\"main.c\":", "main.c:1\n"};
        for (size_t i = 0U; i < sizeof invalid / sizeof invalid[0]; ++i)
            CHECK(Refused(invalid[i], strlen(invalid[i]), UMI_STATUS_INVALID_ARGUMENT) == 0);
        const char embedded[] = {'a', '\0', ':', '2'};
        CHECK(Refused(embedded, sizeof embedded, UMI_STATUS_INVALID_ARGUMENT) == 0);
    } else if (strcmp(mode, "bounds") == 0) {
        char input[UMI_QUICK_OPEN_QUERY_MAX + 2U];
        memset(input, 'x', sizeof input); input[UMI_QUICK_OPEN_QUERY_MAX] = '\0';
        CHECK(Accepted(input, input, 0U, 0U) == 0);
        CHECK(Refused(input, UMI_QUICK_OPEN_QUERY_MAX + 1U, UMI_STATUS_CAPACITY_EXCEEDED) == 0);
        const char bounded[] = {'f', ':', '2', 'x'};
        UmiQuickOpenQuery result;
        CHECK(UmiQuickOpenQueryParse(bounded, 3U, &result) == UMI_STATUS_OK);
        CHECK(strcmp(result.filter, "f") == 0 && result.location.line == 2U);
        memset(input, '1', sizeof input); input[0] = 'f'; input[1] = ':';
        CHECK(Refused(input, UMI_DOCUMENT_LOCATION_TEXT_MAX + 3U, UMI_STATUS_CAPACITY_EXCEEDED) == 0);
    } else if (strcmp(mode, "overflow") == 0) {
        char input[128];
        int count = snprintf(input, sizeof input, "f:%zu", SIZE_MAX);
        CHECK(count > 0 && (size_t)count + 1U < sizeof input);
        CHECK(Accepted(input, "f", SIZE_MAX, 1U) == 0);
        input[count] = '0'; input[count + 1] = '\0';
        CHECK(Refused(input, strlen(input), UMI_STATUS_INVALID_ARGUMENT) == 0);
        count = snprintf(input, sizeof input, "f:1:%zu0", SIZE_MAX);
        CHECK(count > 0 && (size_t)count < sizeof input);
        CHECK(Refused(input, strlen(input), UMI_STATUS_INVALID_ARGUMENT) == 0);
    } else if (strcmp(mode, "arguments") == 0) {
        CHECK(Refused(NULL, 0U, UMI_STATUS_INVALID_ARGUMENT) == 0);
        CHECK(UmiQuickOpenQueryParse("f", 1U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else return 2;
    return 0;
}
