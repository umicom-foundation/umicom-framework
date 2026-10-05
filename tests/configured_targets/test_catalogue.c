/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/configured_targets/test_catalogue.c
 * PURPOSE: Exercise reply selection, project identity and atomic profile adoption.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
#endif
#define CHECK(test)                                                                                          \
    do                                                                                                       \
    {                                                                                                        \
        if (!(test))                                                                                         \
        {                                                                                                    \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #test);                                       \
            failed = 1;                                                                                      \
            goto cleanup;                                                                                    \
        }                                                                                                    \
    } while (0)
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    int failed = 0, owned = 0;
    char temporary[UMI_BUILD_PATH_CAPACITY], root[UMI_BUILD_PATH_CAPACITY], build[UMI_BUILD_PATH_CAPACITY],
        leaf[160];
    UmiProjectTargetCatalogue *catalogue = NULL;
    UmiProjectTargetSummary summary;
    UmiProjectTargetChoice choice;
#ifdef _WIN32
    unsigned long process = (unsigned long)GetCurrentProcessId();
#else
    unsigned long process = (unsigned long)getpid();
#endif
    int length = snprintf(leaf, sizeof(leaf), "umicom-targets-%lu-%s", process, mode);
    CHECK(length > 0 && (size_t)length < sizeof(leaf) && strchr(mode, '/') == NULL &&
          strchr(mode, '\\') == NULL);
    /* A file-API catalogue must be discoverable in a Unicode project folder,
     * not merely readable when its individual files have ASCII parents. */
    if (strcmp(mode, "unicode-root") == 0) {
        size_t used = strlen(leaf);
        const char suffix[] = "-caf\xc3\xa9-\xe6\x96\x87";
        CHECK(used + sizeof(suffix) <= sizeof(leaf));
        memcpy(leaf + used, suffix, sizeof(suffix));
    }
    CHECK(umi_fs_temp_directory(temporary, sizeof(temporary)) == UMI_STATUS_OK);
    CHECK(umi_path_join(temporary, leaf, root, sizeof(root)) == UMI_STATUS_OK);
    CHECK(!umi_fs_exists(root));
    CHECK(umi_fs_make_directories(root) == UMI_STATUS_OK);
    owned = 1;
    CHECK(FixtureReply(root, mode, build) == UMI_STATUS_OK);
    if (strcmp(mode, "cancelled") == 0 || strcmp(mode, "cancel-reset") == 0) {
        UmiCancellationToken *cancel = NULL;
        CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
        umi_cancellation_token_request(cancel);
        UmiStatus cancelled = UmiProjectTargetCatalogueReadCancellable(root, build, "Debug", cancel, &catalogue);
        if (strcmp(mode, "cancel-reset") == 0 && cancelled == UMI_STATUS_CANCELLED && catalogue == NULL) {
            umi_cancellation_token_reset(cancel);
            cancelled = UmiProjectTargetCatalogueReadCancellable(root, build, "Debug", cancel, &catalogue);
        }
        umi_cancellation_token_destroy(cancel);
        CHECK(cancelled == (strcmp(mode, "cancel-reset") == 0 ? UMI_STATUS_OK : UMI_STATUS_CANCELLED));
        CHECK((catalogue != NULL) == (strcmp(mode, "cancel-reset") == 0));
        goto cleanup;
    }
    UmiStatus status =
        UmiProjectTargetCatalogueRead(strcmp(mode, "relative-root") == 0 ? "relative" : root, build,
                                      strcmp(mode, "wrong-config") == 0 ? "Release" : "Debug", &catalogue);
    struct
    {
        const char *name;
        UmiStatus status;
    } errors[] = {{"syntax", UMI_STATUS_PARSE_ERROR},
                  {"traversal", UMI_STATUS_PARSE_ERROR},
                  {"duplicate-field", UMI_STATUS_PARSE_ERROR},
                  {"unsupported", UMI_STATUS_PARSE_ERROR},
                  {"new-error", UMI_STATUS_INVALID_STATE},
                  {"wrong-root", UMI_STATUS_INVALID_STATE},
                  {"wrong-build", UMI_STATUS_INVALID_STATE},
                  {"identity", UMI_STATUS_INVALID_STATE},
                  {"abstract", UMI_STATUS_INVALID_STATE},
                  {"duplicate-target", UMI_STATUS_ALREADY_EXISTS},
                  {"duplicate-config", UMI_STATUS_ALREADY_EXISTS},
                  {"no-model", UMI_STATUS_NOT_FOUND},
                  {"missing-model", UMI_STATUS_NOT_FOUND},
                  {"missing-target", UMI_STATUS_NOT_FOUND},
                  {"wrong-config", UMI_STATUS_NOT_FOUND},
                  {"bad-artifact", UMI_STATUS_PARSE_ERROR},
                  {"nul-artifact", UMI_STATUS_PARSE_ERROR},
                  {"byte-limit", UMI_STATUS_CAPACITY_EXCEEDED},
                  {"relative-root", UMI_STATUS_INVALID_ARGUMENT}};
    for (size_t i = 0U; i < sizeof(errors) / sizeof(errors[0]); ++i)
        if (strcmp(mode, errors[i].name) == 0)
        {
            CHECK(status == errors[i].status && catalogue == NULL);
            goto cleanup;
        }
    CHECK(status == UMI_STATUS_OK && catalogue != NULL);
    CHECK(UmiProjectTargetCatalogueSummary(catalogue, &summary) == UMI_STATUS_OK);
    CHECK(umi_path_equal(summary.source_directory, root) && umi_path_equal(summary.build_directory, build));
    CHECK(strstr(summary.index_file, "index-200.json") != NULL);
    CHECK(summary.count == (strcmp(mode, "empty") == 0 ? 0U : 1U));
    if (strcmp(mode, "empty") == 0)
    {
        CHECK(UmiProjectTargetCatalogueAt(catalogue, 0U, &choice) == UMI_STATUS_NOT_FOUND);
        goto cleanup;
    }
    CHECK(UmiProjectTargetCatalogueAt(catalogue, 0U, &choice) == UMI_STATUS_OK);
    CHECK(strcmp(choice.name, "notes") == 0 && strcmp(choice.identity, "notes::test") == 0);
    UmiBuildProfile profile, selected, before;
    umi_build_profile_init(&profile);
    strcpy(profile.source_directory, root);
    strcpy(profile.build_directory, build);
    strcpy(profile.run_argument, "an argument to preserve");
    before = profile;
    CHECK(UmiProjectTargetCatalogueSelect(catalogue, 0U, UMI_PROJECT_TARGET_BUILD, &profile, &selected) ==
          UMI_STATUS_OK);
    strcpy(before.build_target, "notes");
    CHECK(memcmp(&selected, &before, sizeof(before)) == 0);
    before = profile;
    selected = profile;
    if (strcmp(mode, "stale-profile") == 0)
        strcpy(profile.configuration, "Release");
    if (strcmp(mode, "build-preset") == 0)
        strcpy(profile.build_preset, "debug-build");
    UmiProjectTargetSelection kind =
        strcmp(mode, "build-preset") == 0 ? UMI_PROJECT_TARGET_BUILD : UMI_PROJECT_TARGET_PROGRAM;
    status = UmiProjectTargetCatalogueSelect(catalogue, 0U, kind, &profile, &selected);
    if (strcmp(mode, "library") == 0 || strcmp(mode, "ambiguous") == 0 || strcmp(mode, "no-artifact") == 0 ||
        strcmp(mode, "launcher") == 0 || strcmp(mode, "stale-profile") == 0 ||
        strcmp(mode, "build-preset") == 0)
    {
        CHECK(status == UMI_STATUS_INVALID_STATE && memcmp(&selected, &before, sizeof(before)) == 0);
    }
    else
    {
        CHECK(status == UMI_STATUS_OK);
        char expected[UMI_BUILD_PATH_CAPACITY];
        CHECK(umi_path_join(build, "bin/notes.exe", expected, sizeof(expected)) == UMI_STATUS_OK);
        CHECK(strcmp(selected.run_program, expected) == 0);
        strcpy(before.run_program, expected);
        CHECK(memcmp(&selected, &before, sizeof(before)) == 0);
        CHECK(UmiProjectTargetCatalogueSelect(catalogue, 0U, UMI_PROJECT_TARGET_PROGRAM, &profile,
                                              &profile) == UMI_STATUS_OK);
        CHECK(memcmp(&profile, &before, sizeof(before)) == 0);
    }
    if (strcmp(mode, "multi") == 0)
    {
        UmiProjectTargetCatalogueDestroy(catalogue);
        catalogue = NULL;
        CHECK(UmiProjectTargetCatalogueRead(root, build, "Release", &catalogue) == UMI_STATUS_OK);
        CHECK(UmiProjectTargetCatalogueSummary(catalogue, &summary) == UMI_STATUS_OK && summary.count == 0U);
    }
cleanup:
    UmiProjectTargetCatalogueDestroy(catalogue);
    /* Remove only the uniquely named tree this process created. Existing paths
     * never become owned, including after an interrupted earlier fixture. */
    if (owned && umi_fs_remove_tree(root) != UMI_STATUS_OK)
        failed = 1;
    return failed;
}
