/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_modules/test_registration.c
 * PURPOSE: Check application registration conflicts and incomplete source inventories.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/scaffold/application_catalogue.h"
#include <stdio.h>
#include <string.h>
#define CHECK(value) do { if (!(value)) { \
    (void)fprintf(stderr, "line %d: %s\n", __LINE__, #value); failed = 1; goto cleanup; \
} } while (0)

/* The fixture owns a manifest and a Git configuration file, not a Git checkout.
 * These assertions therefore cannot accidentally stage or publish a repository. */
int main(int argc, char **argv)
{
    int failed = 0;
    UmiApplicationCatalogue *catalogue = NULL;
    CHECK(argc == 4);
    UmiApplicationCatalogueIssue issue;
    CHECK(UmiApplicationCatalogueLoad(argv[2], argv[3], &catalogue, &issue) == UMI_STATUS_OK);
    char manifest[UMI_PATH_CAPACITY];
    CHECK(umi_path_join(argv[2], "candidate.yaml", manifest, sizeof manifest) == UMI_STATUS_OK);
    UmiApplicationModuleDescriptor module;
    CHECK(umi_application_module_load(manifest, &module) == UMI_STATUS_OK);
    UmiApplicationCandidateReport report;
    const char *path = "applications/forecast";
    if (strcmp(argv[1], "null") == 0) {
        CHECK(UmiApplicationCatalogueCheckCandidate(NULL, path, &module, &report) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(report.reason == UMI_APPLICATION_CANDIDATE_INVALID && report.checked == 0U);
        CHECK(UmiApplicationCatalogueCheckCandidate(catalogue, NULL, &module, &report) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiApplicationCatalogueCheckCandidate(catalogue, path, NULL, &report) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiApplicationCatalogueCheckCandidate(catalogue, path, &module, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(strcmp(UmiApplicationCandidateReasonText((UmiApplicationCandidateReason)999),
            UmiApplicationCandidateReasonText(UMI_APPLICATION_CANDIDATE_INVALID)) == 0);
        goto cleanup;
    }
    UmiStatus expected = UMI_STATUS_ALREADY_EXISTS;
    UmiApplicationCandidateReason reason = UMI_APPLICATION_CANDIDATE_READY;
    if (strcmp(argv[1], "ready") == 0 || strcmp(argv[1], "empty") == 0) expected = UMI_STATUS_OK;
    else if (strcmp(argv[1], "path") == 0) { path = "applications/weather"; reason = UMI_APPLICATION_CANDIDATE_PATH; }
    else if (strcmp(argv[1], "id") == 0) reason = UMI_APPLICATION_CANDIDATE_ID;
    else if (strcmp(argv[1], "option") == 0) reason = UMI_APPLICATION_CANDIDATE_OPTION;
    else if (strcmp(argv[1], "component") == 0) reason = UMI_APPLICATION_CANDIDATE_COMPONENT;
    else if (strcmp(argv[1], "executable") == 0) reason = UMI_APPLICATION_CANDIDATE_EXECUTABLE;
    else if (strcmp(argv[1], "unavailable") == 0 || strcmp(argv[1], "malformed") == 0) {
        expected = UMI_STATUS_INVALID_STATE; reason = UMI_APPLICATION_CANDIDATE_UNAVAILABLE;
    } else if (strcmp(argv[1], "capacity") == 0) {
        expected = UMI_STATUS_CAPACITY_EXCEEDED; reason = UMI_APPLICATION_CANDIDATE_CAPACITY;
    } else if (strcmp(argv[1], "traversal") == 0 || strcmp(argv[1], "nested") == 0 ||
        strcmp(argv[1], "uppercase") == 0) {
        expected = UMI_STATUS_INVALID_ARGUMENT; reason = UMI_APPLICATION_CANDIDATE_INVALID;
        if (strcmp(argv[1], "traversal") == 0) path = "applications/../forecast";
        if (strcmp(argv[1], "nested") == 0) path = "applications/forecast/nested";
        if (strcmp(argv[1], "uppercase") == 0) path = "applications/Forecast";
    } else if (strcmp(argv[1], "invalid_descriptor") == 0) {
        expected = UMI_STATUS_PARSE_ERROR; reason = UMI_APPLICATION_CANDIDATE_INVALID;
        memset(module.id, 'x', sizeof module.id);
    } else { failed = 2; goto cleanup; }
    UmiApplicationModuleDescriptor saved = module;
    CHECK(UmiApplicationCatalogueCheckCandidate(catalogue, path, &module, &report) == expected);
    CHECK(report.status == expected && report.reason == reason);
    CHECK(memcmp(&saved, &module, sizeof module) == 0);
    if (expected == UMI_STATUS_ALREADY_EXISTS || reason == UMI_APPLICATION_CANDIDATE_UNAVAILABLE)
        CHECK(strcmp(report.conflicting_path, "applications/weather") == 0);
    else CHECK(report.conflicting_path[0] == '\0');
    if (strcmp(argv[1], "ready") == 0) CHECK(report.checked == 1U);
    if (strcmp(argv[1], "empty") == 0) CHECK(report.checked == 0U);
    if (strcmp(argv[1], "capacity") == 0) CHECK(report.checked == UMI_APPLICATION_CATALOGUE_LIMIT);
cleanup:
    UmiApplicationCatalogueDestroy(catalogue);
    return failed;
}
