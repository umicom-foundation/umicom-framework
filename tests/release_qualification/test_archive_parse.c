/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/release_qualification/test_archive_parse.c
 * Purpose: Reject corrupted or inconsistent saved qualification without trusting claimed counters.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/distribution/runtime/qualification_archive.h"
#include "umicom/distribution/runtime/qualification_report.h"
#include <stdlib.h>
#include <string.h>
static int SetField(char **text, const char *row, size_t column, const char *replacement)
{
    char *line = strstr(*text, row);
    REQUIRE(line != NULL && (line == *text || line[-1] == '\n'));
    char *start = line;
    for (size_t i = 0U; i < column; ++i) {
        start = strchr(start, '\t'); REQUIRE(start != NULL); ++start;
    }
    char *end = start;
    while (*end != '\t' && *end != '\n' && *end != '\0') ++end;
    size_t prefix = (size_t)(start - *text), suffix = strlen(end), length = strlen(replacement);
    char *copy = malloc(prefix + length + suffix + 1U); REQUIRE(copy != NULL);
    memcpy(copy, *text, prefix); memcpy(copy + prefix, replacement, length);
    memcpy(copy + prefix + length, end, suffix + 1U); free(*text); *text = copy; return 0;
}
int main(int argc, char **argv)
{
    REQUIRE(argc == 2); const char *name = argv[1];
    bool open = strcmp(name, "open") == 0, noTests = strcmp(name, "no-tests") == 0;
    QualificationFixture f;
    REQUIRE(QualificationStart(&f, noTests ? 0U : strcmp(name, "large") == 0 ? 5000U : 2U, false) == 0);
    bool process = strcmp(name, "process") == 0;
    if (process) {
        UmiReleaseQualificationResult build = QualificationResult(UMI_QUALIFICATION_PASSED);
        REQUIRE(UmiReleaseQualificationRecordBuild(f.run, &f.identity, &build) == UMI_STATUS_OK);
        UmiReleaseQualificationNamedResult results[2] = {
            {.name = "test.000000", .result = build}, {.name = "test.000001", .result = build}};
        results[0].result.exitCodeKnown = false; results[1].result.exitCodeKnown = false;
        REQUIRE(UmiReleaseQualificationRecordTests(f.run, &f.identity, results, 2U, &build) == UMI_STATUS_OK);
    } else if (!open) REQUIRE(QualificationRecordAll(&f) == 0);
    bool interrupted = strcmp(name, "interrupted") == 0, unobserved = strcmp(name, "unobserved") == 0;
    if (interrupted) REQUIRE(UmiReleaseQualificationInterrupt(f.run, &f.identity, UMI_STATUS_CANCELLED) == UMI_STATUS_OK);
    if (!open) REQUIRE(UmiReleaseQualificationClose(f.run, &f.identity, unobserved ? NULL : f.before,
        unobserved ? NULL : f.toolchain) == UMI_STATUS_OK);
    size_t capacity = 0U;
    REQUIRE(UmiReleaseQualificationFormatReport(f.run, NULL, 0U, &capacity) == UMI_STATUS_OK);
    char *text = malloc(capacity + 20U); REQUIRE(text != NULL);
    REQUIRE(UmiReleaseQualificationFormatReport(f.run, text, capacity, &capacity) == UMI_STATUS_OK);
    bool valid = open || noTests || process || interrupted || unobserved ||
        strcmp(name, "passed") == 0 || strcmp(name, "large") == 0;
    if (strcmp(name, "version") == 0) REQUIRE(SetField(&text, "UMICOM-QUALIFICATION", 1U, "2") == 0);
    else if (strcmp(name, "identity") == 0) {
        char *plan = strchr(strstr(text, "identity\t") + 9, '\t'); REQUIRE(plan != NULL);
        plan[1] = plan[1] == '0' ? '1' : '0';
    } else if (strcmp(name, "counts") == 0) REQUIRE(SetField(&text, "summary\t", 3U, "0") == 0);
    else if (strcmp(name, "ready") == 0) REQUIRE(SetField(&text, "summary\t", 15U, "0") == 0);
    else if (strcmp(name, "numeric-overflow") == 0) REQUIRE(SetField(&text, "selection\t", 1U, "18446744073709551616") == 0);
    else if (strcmp(name, "oversized-count") == 0) REQUIRE(SetField(&text, "summary\t", 1U, "200001") == 0);
    else if (strcmp(name, "oversized-name") == 0) {
        size_t size = ((size_t)UMI_RELEASE_INVENTORY_FIELD_LIMIT + 1U) * 2U;
        char *encoded = malloc(size + 1U); REQUIRE(encoded != NULL);
        memset(encoded, '6', size); encoded[size] = '\0';
        REQUIRE(SetField(&text, "target\t", 1U, encoded) == 0); free(encoded);
    }
    else if (strcmp(name, "leading-zero") == 0) REQUIRE(SetField(&text, "selection\t", 1U, "01") == 0);
    else if (strcmp(name, "invalid-mask") == 0) REQUIRE(SetField(&text, "selection\t", 2U, "128") == 0);
    else if (strcmp(name, "source-pair") == 0) REQUIRE(SetField(&text, "source-after\t", 1U, "") == 0);
    else if (strcmp(name, "open-after") == 0) REQUIRE(SetField(&text, "summary\t", 13U, "0") == 0);
    else if (strcmp(name, "unknown-outcome") == 0) REQUIRE(SetField(&text, "build\t", 1U, "succeeded") == 0);
    else if (strcmp(name, "invalid-pass") == 0) REQUIRE(SetField(&text, "build\t", 3U, "1") == 0);
    else if (strcmp(name, "unknown-exit") == 0) REQUIRE(SetField(&text, "build\t", 3U, "-") == 0);
    else if (strcmp(name, "invalid-name") == 0) REQUIRE(SetField(&text, "target\t", 1U, "00") == 0);
    else if (strcmp(name, "duplicate") == 0 || strcmp(name, "unsorted") == 0) {
        char *first = strstr(text, "\ntest\t"); REQUIRE(first != NULL);
        char *second = strstr(first + 1, "\ntest\t"); REQUIRE(second != NULL);
        char *firstEnd = strchr(first + 6, '\t'), *secondEnd = strchr(second + 6, '\t');
        REQUIRE(firstEnd != NULL && secondEnd != NULL && firstEnd - first == secondEnd - second);
        size_t length = (size_t)(firstEnd - (first + 6));
        if (strcmp(name, "duplicate") == 0) memcpy(second + 6, first + 6, length);
        else { char *copy = malloc(length); REQUIRE(copy != NULL); memcpy(copy, first + 6, length);
            memcpy(first + 6, second + 6, length); memcpy(second + 6, copy, length); free(copy); }
    } else if (strcmp(name, "trailing") == 0) strcat(text, "extra\n");
    else if (strcmp(name, "unknown-row") == 0) memcpy(strstr(text, "inventory\t"), "inventorx", 9U);
    else REQUIRE(valid || strcmp(name, "truncated") == 0 || strcmp(name, "embedded-nul") == 0 ||
                 strcmp(name, "invalid-arguments") == 0);
    size_t length = strlen(text);
    if (strcmp(name, "truncated") == 0) --length;
    if (strcmp(name, "embedded-nul") == 0) text[length / 2U] = '\0';
    UmiQualificationView *view = NULL;
    UmiQualificationArchiveReceipt receipt, before;
    memset(&receipt, 0xa5, sizeof(receipt)); memcpy(&before, &receipt, sizeof(receipt));
    UmiStatus status = UmiQualificationArchiveParse(
        strcmp(name, "invalid-arguments") == 0 ? NULL : text, length, &view, &receipt);
    if (valid) {
        REQUIRE(status == UMI_STATUS_OK && view != NULL);
        UmiQualificationViewSummary summary;
        REQUIRE(UmiQualificationViewDescribe(view, &summary) == UMI_STATUS_OK && summary.savedReport);
        REQUIRE(summary.results.readyForOwnerReview == (!open && !noTests && !interrupted && !unobserved));
        REQUIRE(strcmp(summary.identity.plan, f.identity.plan) == 0);
        char digest[65]; REQUIRE(UmiSha256Buffer(text, length, digest) == UMI_STATUS_OK);
        REQUIRE(strcmp(receipt.reportSha256, digest) == 0 && receipt.bytes == length);
        /* The view owns names and identity after the input buffer and run vanish. */
        QualificationDestroy(&f); free(text); text = NULL;
        REQUIRE(strncmp(UmiQualificationViewStateText(view), "Saved", 5U) == 0);
        REQUIRE(UmiQualificationViewAt(view, 2U) != NULL &&
            strcmp(UmiQualificationViewAt(view, 2U)->name, "library") == 0);
    } else {
        REQUIRE(status != UMI_STATUS_OK && view == NULL);
        REQUIRE(memcmp(&receipt, &before, sizeof(receipt)) == 0);
        QualificationDestroy(&f);
    }
    UmiQualificationViewDestroy(view); free(text); return 0;
}
