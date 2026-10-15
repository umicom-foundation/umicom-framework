/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_modules/test_scaffold.c
 * PURPOSE: Check source previews, exclusive application creation and retained failure state.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/scaffold/application_module.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/input_file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(condition) do { if (!(condition)) { \
    (void)fprintf(stderr, "line %d: %s\n", __LINE__, #condition); return 1; \
} } while (0)

/* Each CTest invocation receives a fresh, private directory. Tests never
 * remove application source or rely on a user's Git identity or credentials. */
int main(int argc, char **argv)
{
    UmiApplicationScaffoldRequest request = {0};
    UmiApplicationScaffoldPlan *plan = NULL;
    UmiApplicationScaffoldSummary summary;
    UmiApplicationScaffoldFile view;
    UmiApplicationScaffoldResult result;
    UmiStatus status, expected = UMI_STATUS_OK;
    char parent[UMI_PATH_CAPACITY], templates[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
    char application[UMI_PATH_CAPACITY], long_text[260];
    const char *mode;
    CHECK(argc == 3);
    mode = argv[1];
    CHECK(umi_path_join(argv[2], "output", parent, sizeof(parent)) == UMI_STATUS_OK);
    CHECK(umi_path_join(argv[2], "templates", templates, sizeof(templates)) == UMI_STATUS_OK);
    CHECK(umi_path_join(parent, "weather", application, sizeof(application)) == UMI_STATUS_OK);
    request.template_root = templates;
    request.destination_parent = parent;
    request.slug = "weather";
    request.application_name = "Weather";
    request.application_id = "org.example.weather";
    request.gtk_frontend = strcmp(mode, "console") != 0;
    memset(long_text, 'a', sizeof(long_text) - 1U);
    long_text[sizeof(long_text) - 1U] = '\0';

    if (strcmp(mode, "null") == 0) {
        CHECK(umi_application_scaffold_plan(NULL, &plan) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(plan == NULL);
        CHECK(umi_application_scaffold_plan(&request, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_application_scaffold_apply(NULL, &result) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(result.created_files == 0U);
        CHECK(umi_application_scaffold_summary(NULL, &summary) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(summary.file_count == 0U);
        CHECK(umi_application_scaffold_file(NULL, 0U, &view) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(view.text == NULL);
        umi_application_scaffold_destroy(NULL);
        return 0;
    }
    if (strcmp(mode, "slug") == 0) request.slug = "../weather";
    if (strcmp(mode, "case") == 0) request.slug = "Weather";
    if (strcmp(mode, "device") == 0) request.slug = "con";
    if (strcmp(mode, "name") == 0) request.application_name = "Weather \"quoted\"";
    if (strcmp(mode, "identity") == 0) request.application_id = "org..weather";
    if (strcmp(mode, "identity_segment") == 0) request.application_id = "org.1weather";
    if (strcmp(mode, "long_slug") == 0) request.slug = long_text;
    if (strcmp(mode, "long_name") == 0) request.application_name = long_text;
    if (strcmp(mode, "long_id") == 0) request.application_id = long_text;
    if (strcmp(mode, "relative") == 0) request.destination_parent = "output";
    if (strcmp(mode, "slug") == 0 || strcmp(mode, "case") == 0 ||
        strcmp(mode, "device") == 0 || strcmp(mode, "name") == 0 ||
        strcmp(mode, "identity") == 0 || strcmp(mode, "identity_segment") == 0 ||
        strcmp(mode, "long_slug") == 0 || strcmp(mode, "long_name") == 0 ||
        strcmp(mode, "long_id") == 0 || strcmp(mode, "relative") == 0)
        expected = UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(mode, "missing_parent") == 0) {
        request.destination_parent = application;
        expected = UMI_STATUS_NOT_FOUND;
    }
    if (strcmp(mode, "collision") == 0) {
        CHECK(umi_fs_make_directories(application) == UMI_STATUS_OK);
        expected = UMI_STATUS_ALREADY_EXISTS;
    }
    if (strcmp(mode, "missing_template") == 0) {
        request.template_root = parent;
        expected = UMI_STATUS_NOT_FOUND;
    }
    if (strcmp(mode, "unknown_token") == 0 || strcmp(mode, "nul") == 0 ||
        strcmp(mode, "oversize") == 0) {
        CHECK(umi_path_join(templates, "README.md.in", path, sizeof(path)) == UMI_STATUS_OK);
        if (strcmp(mode, "unknown_token") == 0) {
            CHECK(umi_fs_write_text(path, "@UNKNOWN@\n") == UMI_STATUS_OK);
            expected = UMI_STATUS_PARSE_ERROR;
        } else if (strcmp(mode, "nul") == 0) {
            const char bytes[] = {'a', '\0', 'b'};
            CHECK(umi_fs_write_bytes(path, bytes, sizeof(bytes)) == UMI_STATUS_OK);
            expected = UMI_STATUS_PARSE_ERROR;
        } else {
            char *bytes = malloc(UMI_APPLICATION_SCAFFOLD_TEXT_LIMIT + 1U);
            CHECK(bytes != NULL);
            memset(bytes, 'a', UMI_APPLICATION_SCAFFOLD_TEXT_LIMIT + 1U);
            CHECK(umi_fs_write_bytes(path, bytes, UMI_APPLICATION_SCAFFOLD_TEXT_LIMIT + 1U) == UMI_STATUS_OK);
            free(bytes);
            expected = UMI_STATUS_CAPACITY_EXCEEDED;
        }
    }
    if (strcmp(mode, "template_identity") == 0) {
        char *manifest = NULL;
        CHECK(umi_path_join(templates, "application.umicom.yaml.in", path, sizeof(path)) == UMI_STATUS_OK);
        CHECK(umi_fs_read_text(path, &manifest, NULL) == UMI_STATUS_OK);
        char *token = strstr(manifest, "@ID@");
        CHECK(token != NULL);
        /* A valid but different identity must not silently replace the
         * request. Keep the mutation small enough to preserve its allocation. */
        memcpy(token, "a.bc", 4U);
        CHECK(umi_fs_write_text(path, manifest) == UMI_STATUS_OK);
        umi_fs_free_text(manifest);
        expected = UMI_STATUS_INVALID_STATE;
    }
    status = umi_application_scaffold_plan(&request, &plan);
    CHECK(status == expected);
    if (status != UMI_STATUS_OK) {
        CHECK(plan == NULL);
        if (strcmp(mode, "collision") != 0) CHECK(!umi_fs_exists(application));
        return 0;
    }
    CHECK(plan != NULL);
    CHECK(umi_application_scaffold_summary(plan, &summary) == UMI_STATUS_OK);
    CHECK(summary.file_count == 11U && summary.byte_count > 0U && !summary.apply_started);
    CHECK(strcmp(summary.module.option, "UMICOM_APPLICATIONS_BUILD_WEATHER") == 0);
    CHECK(strcmp(summary.module.id, request.application_id) == 0);
    CHECK(strcmp(summary.root, application) == 0);
    CHECK(!umi_fs_exists(application));
    CHECK(umi_application_scaffold_file(plan, summary.file_count, &view) == UMI_STATUS_NOT_FOUND);
    CHECK(view.text == NULL);
    if (strcmp(mode, "console") == 0) {
        CHECK(summary.module.native_executable[0] == '\0');
        CHECK(strcmp(summary.module.console_executable, "umicom-weather") == 0);
    } else CHECK(strcmp(summary.module.native_executable, "umicom-weather") == 0);
    if (strcmp(mode, "preview") == 0 || strcmp(mode, "console") == 0) {
        umi_application_scaffold_destroy(plan);
        CHECK(!umi_fs_exists(application));
        return 0;
    }
    if (strcmp(mode, "collision_after") == 0) {
        CHECK(umi_fs_make_directories(application) == UMI_STATUS_OK);
        CHECK(umi_path_join(application, "keep.txt", path, sizeof(path)) == UMI_STATUS_OK);
        CHECK(umi_fs_write_text(path, "existing source\n") == UMI_STATUS_OK);
        CHECK(umi_application_scaffold_apply(plan, &result) == UMI_STATUS_ALREADY_EXISTS);
        CHECK(result.created_files == 0U && result.created_directories == 0U);
        char *saved = NULL;
        CHECK(umi_fs_read_text(path, &saved, NULL) == UMI_STATUS_OK);
        CHECK(strcmp(saved, "existing source\n") == 0);
        umi_fs_free_text(saved);
    } else {
        if (strcmp(mode, "snapshot") == 0) {
            CHECK(umi_path_join(templates, "README.md.in", path, sizeof(path)) == UMI_STATUS_OK);
            CHECK(umi_fs_write_text(path, "changed after review\n") == UMI_STATUS_OK);
        }
        CHECK(umi_application_scaffold_apply(plan, &result) == UMI_STATUS_OK);
        CHECK(result.created_directories == 4U && result.created_files == 11U &&
            result.completed_files == 11U);
        /* Compare every created file with the reviewed bytes, not just its
         * existence. This also checks that later template edits are irrelevant. */
        for (size_t i = 0U; i < summary.file_count; ++i) {
            unsigned char *bytes = NULL; size_t length = 0U;
            CHECK(umi_application_scaffold_file(plan, i, &view) == UMI_STATUS_OK);
            CHECK(umi_path_join(application, view.relative_path, path, sizeof(path)) == UMI_STATUS_OK);
            CHECK(UmiInputFileRead(path, UMI_APPLICATION_SCAFFOLD_TEXT_LIMIT, &bytes, &length) == UMI_STATUS_OK);
            CHECK(length == view.length && memcmp(bytes, view.text, length) == 0);
            UmiInputFileFree(bytes);
        }
        CHECK(umi_path_join(application, ".git", path, sizeof(path)) == UMI_STATUS_OK);
        CHECK(!umi_fs_exists(path));
    }
    CHECK(umi_application_scaffold_summary(plan, &summary) == UMI_STATUS_OK);
    CHECK(summary.apply_started);
    CHECK(umi_application_scaffold_apply(plan, &result) == UMI_STATUS_INVALID_STATE);
    CHECK(result.created_files == 0U);
    umi_application_scaffold_destroy(plan);
    return 0;
}
