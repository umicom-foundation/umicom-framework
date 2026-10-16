/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_operations/test_publication_report.c
 * PURPOSE: Check complete JSON evidence, Unicode boundaries and failure-preserving output.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/publication_report.h"
#include "umicom/protocol/json.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(condition) do { if (!(condition)) { \
    (void)fprintf(stderr, "Report check failed at line %d: %s\n", __LINE__, #condition); \
    goto finish; } } while (0)

int main(void)
{
    UmiRepositoryPublicationReport *report = calloc(1U, sizeof(*report));
    char *json = NULL;
    char destination[16] = "retain output";
    char original[16];
    size_t required = 0U, exact = 0U;
    int complete = 1, result = EXIT_FAILURE;
    if (!report) return result;
    memcpy(original, destination, sizeof(original));
    report->status = UMI_STATUS_IO_ERROR;
    report->count = report->repository_count = report->blocked_count = 1U;
    report->committed_count = 1U;
    UmiRepositoryPublicationEntry *entry = &report->entries[0];
    entry->parent = SIZE_MAX;
    entry->committed = entry->blocked = 1;
    (void)snprintf(entry->path, sizeof(entry->path), "C:\\work\\caf\xc3\xa9\\\"review\"");
    (void)snprintf(entry->detail, sizeof(entry->detail), "Push failed\nInspect\tlocal commit%c", 1);
    memset(entry->head, 'c', 40U);
    memset(entry->remote_head, 'a', 40U);
    (void)snprintf(report->detail, sizeof(report->detail), "Local commit retained; remote publication failed.");
    CHECK(umi_repository_publication_report_format_json(report,
        UMI_REPOSITORY_PUBLICATION_REPORT_APPLY, NULL, 0U, &required) == UMI_STATUS_OK);
    CHECK(required > sizeof(destination));
    CHECK(umi_repository_publication_report_format_json(report,
        UMI_REPOSITORY_PUBLICATION_REPORT_APPLY, destination, sizeof(destination), &exact) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(exact == required && memcmp(original, destination, sizeof(original)) == 0);
    json = malloc(required);
    CHECK(json != NULL);
    CHECK(umi_repository_publication_report_format_json(report,
        UMI_REPOSITORY_PUBLICATION_REPORT_APPLY, json, required, &exact) == UMI_STATUS_OK);
    CHECK(exact == required && strlen(json) + 1U == required && json[required - 2U] == '\n');
    CHECK(umi_json_get_boolean(json, "publication_completed", &complete) == UMI_STATUS_OK && !complete);
    CHECK(strstr(json, "\\\"review\\\"") && strstr(json, "caf\xc3\xa9"));
    CHECK(strstr(json, "Push failed\\nInspect\\tlocal commit\\u0001"));
    CHECK(strstr(json, "\"branch\":null") && strstr(json, "\"parent\":null"));
    CHECK(strstr(json, "\"committed\":true") && strstr(json, "\"pushed\":false"));
    CHECK(strstr(json, "\"head\":\"cccccccccccccccccccccccccccccccccccccccc\""));
    free(json); json = NULL;

    /* A successful preview remains a preview; only a successful apply can
     * represent a completed publication. Neither result qualifies a build. */
    report->status = UMI_STATUS_OK;
    report->blocked_count = 0U;
    entry->blocked = 0;
    for (int mode = 0; mode < 3; ++mode) {
        CHECK(umi_repository_publication_report_format_json(report,
            (UmiRepositoryPublicationReportMode)mode, NULL, 0U, &required) == UMI_STATUS_OK);
        json = malloc(required);
        CHECK(json != NULL);
        CHECK(umi_repository_publication_report_format_json(report,
            (UmiRepositoryPublicationReportMode)mode, json, required, &exact) == UMI_STATUS_OK);
        CHECK(umi_json_get_boolean(json, "publication_completed", &complete) == UMI_STATUS_OK);
        CHECK(complete == (mode == UMI_REPOSITORY_PUBLICATION_REPORT_APPLY));
        free(json); json = NULL;
    }

    /* Refuse malformed caller-owned snapshots before touching output memory. */
    report->count = UMI_REPOSITORY_PUBLICATION_CAPACITY + 1U;
    CHECK(umi_repository_publication_report_format_json(report,
        UMI_REPOSITORY_PUBLICATION_REPORT_PREVIEW, destination, sizeof(destination), &required) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(required == 0U && memcmp(original, destination, sizeof(original)) == 0);
    report->count = 1U;
    entry->primary = report->count;
    CHECK(umi_repository_publication_report_format_json(report,
        UMI_REPOSITORY_PUBLICATION_REPORT_PREVIEW, NULL, 0U, &required) == UMI_STATUS_INVALID_ARGUMENT);
    entry->primary = 0U;
    entry->parent = report->count;
    CHECK(umi_repository_publication_report_format_json(report,
        UMI_REPOSITORY_PUBLICATION_REPORT_PREVIEW, NULL, 0U, &required) == UMI_STATUS_INVALID_ARGUMENT);
    entry->parent = SIZE_MAX;
    memset(entry->branch, 'x', sizeof(entry->branch));
    CHECK(umi_repository_publication_report_format_json(report,
        UMI_REPOSITORY_PUBLICATION_REPORT_PREVIEW, NULL, 0U, &required) == UMI_STATUS_INVALID_ARGUMENT);
    entry->branch[0] = '\0';
    const char *invalid_utf8[] = {"\xc0\xaf", "\xed\xa0\x80", "\xf4\x90\x80\x80", "\xe2\x82", "\x80"};
    for (size_t i = 0U; i < sizeof(invalid_utf8) / sizeof(invalid_utf8[0]); ++i) {
        (void)snprintf(entry->branch, sizeof(entry->branch), "%s", invalid_utf8[i]);
        CHECK(umi_repository_publication_report_format_json(report,
            UMI_REPOSITORY_PUBLICATION_REPORT_PREVIEW, destination, sizeof(destination), &required) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(required == 0U && memcmp(original, destination, sizeof(original)) == 0);
    }
    entry->branch[0] = '\0';
    CHECK(umi_repository_publication_report_format_json(report,
        (UmiRepositoryPublicationReportMode)99, NULL, 0U, &required) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_repository_publication_report_format_json(NULL,
        UMI_REPOSITORY_PUBLICATION_REPORT_PREVIEW, NULL, 0U, &required) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_repository_publication_report_format_json(report,
        UMI_REPOSITORY_PUBLICATION_REPORT_PREVIEW, NULL, 1U, &required) == UMI_STATUS_INVALID_ARGUMENT);

    /* Maximum row count and maximally escaped paths must still serialize as
     * one complete document instead of truncating the last checkout. */
    memset(report, 0, sizeof(*report));
    report->count = UMI_REPOSITORY_PUBLICATION_CAPACITY;
    report->repository_count = report->count;
    for (size_t i = 0U; i < report->count; ++i) {
        report->entries[i].parent = SIZE_MAX;
        report->entries[i].primary = i;
        memset(report->entries[i].path, 1, sizeof(report->entries[i].path) - 1U);
    }
    CHECK(umi_repository_publication_report_format_json(report,
        UMI_REPOSITORY_PUBLICATION_REPORT_CACHED, NULL, 0U, &required) == UMI_STATUS_OK);
    json = malloc(required);
    CHECK(json != NULL);
    CHECK(umi_repository_publication_report_format_json(report,
        UMI_REPOSITORY_PUBLICATION_REPORT_CACHED, json, required, &exact) == UMI_STATUS_OK);
    CHECK(strlen(json) + 1U == required && strstr(json, "\"primary\":127,"));
    CHECK(strcmp(json + required - 7U, "  ]\n}\n") == 0);
    result = EXIT_SUCCESS;
finish:
    free(json);
    free(report);
    return result;
}
