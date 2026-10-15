/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_modules/test_catalogue.c
 * PURPOSE: Check registered application discovery through the real Git configuration parser.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/scaffold/application_catalogue.h"
#include "umicom/platform/filesystem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(value) do { if (!(value)) { \
    (void)fprintf(stderr, "line %d: %s\n", __LINE__, #value); failed = 1; goto cleanup; \
} } while (0)

/* Fixtures are created outside repositories. Git reads only their .gitmodules;
 * no init, clone, add, commit, checkout, network request or application runs. */
int main(int argc, char **argv)
{
    int failed = 0;
    UmiApplicationCatalogue *catalogue = NULL;
    UmiApplicationCatalogueIssue issue;
    UmiApplicationCatalogueSummary summary;
    CHECK(argc == 4);
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(argv[1], "duplicate_path") == 0) expected = UMI_STATUS_ALREADY_EXISTS;
    if (strcmp(argv[1], "duplicate_id") == 0 || strcmp(argv[1], "duplicate_component") == 0 ||
        strcmp(argv[1], "duplicate_option") == 0 || strcmp(argv[1], "duplicate_executable") == 0)
        expected = UMI_STATUS_INVALID_STATE;
    /* Retain the text-parser expectation. Binary Git records now allow a
     * subsection name with spaces without changing its registered path. */
#if 0
    if (strcmp(argv[1], "traversal") == 0 || strcmp(argv[1], "nested") == 0 ||
        strcmp(argv[1], "nul") == 0 || strcmp(argv[1], "key_space") == 0)
        expected = UMI_STATUS_PARSE_ERROR;
#endif
    if (strcmp(argv[1], "traversal") == 0 || strcmp(argv[1], "nested") == 0 ||
        strcmp(argv[1], "nul") == 0 || strcmp(argv[1], "device") == 0 ||
        strcmp(argv[1], "leading_digit") == 0 || strcmp(argv[1], "trailing_dash") == 0 ||
        strcmp(argv[1], "path_newline") == 0)
        expected = UMI_STATUS_PARSE_ERROR;
    if (strcmp(argv[1], "capacity") == 0) expected = UMI_STATUS_CAPACITY_EXCEEDED;
    if (strcmp(argv[1], "null") == 0) {
        CHECK(UmiApplicationCatalogueLoad(NULL, argv[3], &catalogue, &issue) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(catalogue == NULL && issue.status == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiApplicationCatalogueLoad(argv[2], argv[3], NULL, &issue) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiApplicationCatalogueLoad("relative", argv[3], &catalogue, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiApplicationCatalogueSummaryRead(NULL, &summary) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(summary.count == 0U);
        CHECK(UmiApplicationCatalogueAt(NULL, 0U) == NULL);
        goto cleanup;
    }
    UmiStatus status = UmiApplicationCatalogueLoad(argv[2], argv[3], &catalogue, &issue);
    CHECK(status == expected && issue.status == expected);
    if (expected != UMI_STATUS_OK) {
        CHECK(catalogue == NULL);
        if (expected == UMI_STATUS_INVALID_STATE || expected == UMI_STATUS_ALREADY_EXISTS)
            CHECK(issue.path[0] != '\0' && issue.conflicting_path[0] != '\0');
        goto cleanup;
    }
    CHECK(UmiApplicationCatalogueSummaryRead(catalogue, &summary) == UMI_STATUS_OK);
    CHECK(umi_path_equal(summary.root, argv[2]));
    size_t count = 1U, ready = 1U;
    if (strcmp(argv[1], "empty") == 0 || strcmp(argv[1], "absent") == 0 ||
        strcmp(argv[1], "includes") == 0 || strcmp(argv[1], "unregistered") == 0) count = ready = 0U;
    if (strcmp(argv[1], "missing") == 0 || strcmp(argv[1], "malformed") == 0) ready = 0U;
    CHECK(summary.count == count && summary.ready == ready && summary.unavailable == count - ready);
    CHECK(UmiApplicationCatalogueAt(catalogue, count) == NULL);
    if (count != 0U) {
        const UmiApplicationCatalogueEntry *entry = UmiApplicationCatalogueAt(catalogue, 0U);
        CHECK(entry != NULL && strcmp(entry->path, "applications/weather") == 0);
        if (ready != 0U) {
            CHECK(entry->manifest_status == UMI_STATUS_OK);
            CHECK(strcmp(entry->module.id, "org.example.weather") == 0);
            CHECK(strcmp(entry->module.option, "UMICOM_APPLICATIONS_BUILD_WEATHER") == 0);
        } else CHECK(entry->manifest_status != UMI_STATUS_OK);
    }
cleanup:
    UmiApplicationCatalogueDestroy(catalogue);
    return failed;
}
