/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/os_foundation/test_boot_report_json.c
 *
 * PURPOSE:
 *   Verify schema semantics, shared admission and unchanged outputs for bounded captured boot JSON.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/platform/boot_report_json.h"
#include <stdio.h>
#include <string.h>
#define CHECK(condition) do { if (!(condition)) { fprintf(stderr, "line %d: %s\n", __LINE__, #condition); return 1; } } while (0)
static UmiBootReport Ready(void)
{
    UmiBootReport report = {0};
    report.mode = UMI_BOOT_REPORT_NORMAL;
    report.state = UMI_BOOT_REPORT_READY;
    report.planned = 2U; report.completed = 2U;
    memcpy(report.reason, "none", 5U);
    memcpy(report.sourceId, "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef", 65U);
    return report;
}
static int Refused(const UmiBootReport *before, const UmiBootReport *after, int compare)
{
    char out[UMI_BOOT_REPORT_JSON_CAPACITY], copy[sizeof out];
    memset(out, 'x', sizeof out); memcpy(copy, out, sizeof out);
    size_t length = 731U;
    UmiStatus status = compare ? UmiBootReportComparisonJson(before, after, out, sizeof out, &length) :
        UmiBootReportJson(before, out, sizeof out, &length);
    CHECK(status == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(length == 731U && !memcmp(out, copy, sizeof out));
    return 0;
}
static int Capacity(const UmiBootReport *before, const UmiBootReport *after, int compare)
{
    char expected[UMI_BOOT_REPORT_JSON_CAPACITY], out[sizeof expected];
    size_t required = 0U;
    CHECK((compare ? UmiBootReportComparisonJson(before, after, expected, sizeof expected, &required) :
        UmiBootReportJson(before, expected, sizeof expected, &required)) == UMI_STATUS_OK);
    /* Every insufficient boundary must retain all caller bytes and length,
     * including the boundary that has room for JSON but not its final NUL. */
    for (size_t capacity = 0U; capacity <= required + 1U; ++capacity) {
        memset(out, 'x', sizeof out);
        size_t length = 731U;
        UmiStatus status = compare ? UmiBootReportComparisonJson(before, after, out, capacity, &length) :
            UmiBootReportJson(before, out, capacity, &length);
        if (capacity <= required) {
            CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED && length == 731U);
            for (size_t i = 0U; i < sizeof out; ++i) CHECK(out[i] == 'x');
        } else {
            CHECK(status == UMI_STATUS_OK && length == required);
            CHECK(!memcmp(out, expected, required + 1U));
            CHECK(out[required + 1U] == 'x');
        }
    }
    return 0;
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    UmiBootReport before = Ready(), after = before;
    char out[UMI_BOOT_REPORT_JSON_CAPACITY]; size_t length = 0U;
    int compare = 0;
    const char *expected = NULL;
    if (!strcmp(mode, "capacity")) return Capacity(&before, &after, 0);
    if (!strcmp(mode, "comparison-capacity")) return Capacity(&before, &after, 1);
    if (!strcmp(mode, "nulls")) {
        CHECK(Refused(NULL, &after, 0) == 0 && Refused(NULL, &after, 1) == 0 && Refused(&before, NULL, 1) == 0);
        CHECK(UmiBootReportJson(&before, NULL, sizeof out, &length) == UMI_STATUS_INVALID_ARGUMENT && length == 0U);
        CHECK(UmiBootReportJson(&before, out, sizeof out, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBootReportComparisonJson(&before, &after, NULL, sizeof out, &length) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBootReportComparisonJson(&before, &after, out, sizeof out, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        return 0;
    }
    if (!strncmp(mode, "invalid-", 8U) || !strcmp(mode, "unterminated")) {
        if (!strcmp(mode, "invalid-mode")) before.mode = (UmiBootReportMode)99;
        else if (!strcmp(mode, "invalid-state")) before.state = (UmiBootReportState)99;
        else if (!strcmp(mode, "invalid-counts")) before.completed = 3U;
        else if (!strcmp(mode, "invalid-reason")) memcpy(before.reason, "none\"", 6U);
        else if (!strcmp(mode, "invalid-source")) before.sourceId[0] = 'A';
        else if (!strcmp(mode, "unterminated")) memset(before.reason, 'x', sizeof before.reason);
        else if (!strcmp(mode, "invalid-before")) { before.sourceId[64] = 'x'; compare = 1; }
        else if (!strcmp(mode, "invalid-after")) { after.planned = 17U; compare = 1; }
        else CHECK(0);
        return Refused(&before, &after, compare);
    }
    if (!strcmp(mode, "ready")) {
        expected = "{\"schema_version\":1,\"scope\":\"captured-boot-observation\",\"report\":{\"mode\":\"normal\",\"state\":\"ready\",\"planned\":2,\"completed\":2,\"reason\":\"none\",\"source\":\"0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef\"}}";
    } else if (!strcmp(mode, "starting")) { before.state = UMI_BOOT_REPORT_STARTING; before.completed = 0U; }
    else if (!strcmp(mode, "recovery")) {
        before.mode = UMI_BOOT_REPORT_REQUESTED_RECOVERY; before.state = UMI_BOOT_REPORT_RECOVERY;
        before.completed = 0U; memcpy(before.reason, "requested", 10U);
    } else if (!strcmp(mode, "maximum")) { before.planned = 16U; before.completed = 16U; }
    else if (!strcmp(mode, "all-reasons")) {
        static const char *const reasons[] = {"configuration", "mount", "service-exit", "service-timeout", "service-launch", "report-write", "privilege"};
        before.state = UMI_BOOT_REPORT_RECOVERY;
        for (size_t i = 0U; i < sizeof reasons / sizeof reasons[0]; ++i) {
            memset(before.reason, 0, sizeof before.reason); memcpy(before.reason, reasons[i], strlen(reasons[i]));
            CHECK(UmiBootReportJson(&before, out, sizeof out, &length) == UMI_STATUS_OK);
            CHECK(strstr(out, reasons[i]) != NULL && length == strlen(out));
        }
        return 0;
    } else if (!strcmp(mode, "equal") || !strcmp(mode, "semantic-padding")) {
        compare = 1;
        if (!strcmp(mode, "semantic-padding")) memset(after.reason + 5U, 'x', sizeof after.reason - 5U);
    } else if (!strcmp(mode, "progress")) { compare = 1; before.state = UMI_BOOT_REPORT_STARTING; before.completed = 1U; }
    else if (!strcmp(mode, "source")) { compare = 1; after.sourceId[0] = 'a'; }
    else if (!strcmp(mode, "all-fields")) {
        compare = 1; after.mode = UMI_BOOT_REPORT_REQUESTED_RECOVERY; after.state = UMI_BOOT_REPORT_RECOVERY;
        after.planned = 16U; after.completed = 0U; memcpy(after.reason, "requested", 10U); after.sourceId[0] = 'a';
    } else if (!strcmp(mode, "same-object")) {
        CHECK(UmiBootReportComparisonJson(&before, &before, out, sizeof out, &length) == UMI_STATUS_OK);
        CHECK(strstr(out, "\"same_source\":true,\"changed_fields\":[]") != NULL);
        return 0;
    } else CHECK(0);
/* Retain the initial snapshot expression for review. Byte copies deliberately preserve object representation, because struct assignment need not copy padding. The former implementation is retained for owner review. */
#if 0
    UmiBootReport beforeCopy = before, afterCopy = after;
#endif
    unsigned char beforeCopy[sizeof before], afterCopy[sizeof after];
    memcpy(beforeCopy, &before, sizeof before); memcpy(afterCopy, &after, sizeof after);
    CHECK((compare ? UmiBootReportComparisonJson(&before, &after, out, sizeof out, &length) :
        UmiBootReportJson(&before, out, sizeof out, &length)) == UMI_STATUS_OK);
    CHECK(length == strlen(out) && length < sizeof out);
    CHECK(!memcmp(&before, &beforeCopy, sizeof before) && !memcmp(&after, &afterCopy, sizeof after));
    if (expected) CHECK(!strcmp(out, expected));
    if (!strcmp(mode, "starting")) CHECK(strstr(out, "\"state\":\"starting\",\"planned\":2,\"completed\":0") != NULL);
    if (!strcmp(mode, "recovery")) CHECK(strstr(out, "\"mode\":\"recovery\",\"state\":\"recovery\"") != NULL);
    if (!strcmp(mode, "maximum")) CHECK(strstr(out, "\"planned\":16,\"completed\":16") != NULL);
    if (!strcmp(mode, "equal") || !strcmp(mode, "semantic-padding")) CHECK(strstr(out, "\"same_source\":true,\"changed_fields\":[]") != NULL);
    if (!strcmp(mode, "progress")) CHECK(strstr(out, "\"changed_fields\":[\"state\",\"completed\"]") != NULL);
    if (!strcmp(mode, "source")) CHECK(strstr(out, "\"same_source\":false,\"changed_fields\":[\"source\"]") != NULL);
    if (!strcmp(mode, "all-fields")) CHECK(strstr(out, "[\"mode\",\"state\",\"planned\",\"completed\",\"reason\",\"source\"]") != NULL);
    return 0;
}
