/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/installed_files/test_catalogue.c
 * PURPOSE: Exercise install parsing, file observations and atomic program selection.
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
#define CHECK(value)                                                                                         \
    do                                                                                                       \
    {                                                                                                        \
        if (!(value))                                                                                        \
        {                                                                                                    \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value);                                      \
            failed = 1;                                                                                      \
            goto cleanup;                                                                                    \
        }                                                                                                    \
    } while (0)
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *name = argv[1];
    int failed = 0, owned = 0;
    UmiProjectInstalledFiles *files = NULL;
    UmiCancellationToken *cancel = NULL;
    InstalledFixture fixture;
    char root[UMI_PATH_CAPACITY], temporary[UMI_PATH_CAPACITY], leaf[160];
#ifdef _WIN32
    unsigned long process = (unsigned long)GetCurrentProcessId();
#else
    unsigned long process = (unsigned long)getpid();
#endif
    int length = snprintf(leaf, sizeof(leaf), "umicom-install-%lu-%s", process, name);
    CHECK(length > 0 && (size_t)length < sizeof(leaf) && strchr(name, '/') == NULL &&
          strchr(name, '\\') == NULL);
    CHECK(umi_fs_temp_directory(temporary, sizeof(temporary)) == UMI_STATUS_OK);
    CHECK(umi_path_join(temporary, leaf, root, sizeof(root)) == UMI_STATUS_OK);
    CHECK(!umi_fs_exists(root));
    CHECK(umi_fs_make_directories(root) == UMI_STATUS_OK);
    owned = 1;
    CHECK(InstalledFixtureCreate(root, &fixture) == UMI_STATUS_OK);
    UmiStatus expected = UMI_STATUS_OK;
    char content[UMI_PATH_CAPACITY * 3U];
    content[0] = '\0';
    bool rewrite = false;
    if (strcmp(name, "empty") == 0)
        rewrite = true;
    if (strcmp(name, "crlf") == 0)
    {
        snprintf(content, sizeof(content), "%s\r\n%s\r\n", fixture.program, fixture.data);
        rewrite = true;
    }
    if (strcmp(name, "no-newline") == 0)
    {
        strcpy(content, fixture.program);
        rewrite = true;
    }
    if (strcmp(name, "blank") == 0)
    {
        snprintf(content, sizeof(content), "%s\n\n", fixture.program);
        rewrite = true;
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(name, "cr-only") == 0)
    {
        snprintf(content, sizeof(content), "%s\r", fixture.program);
        rewrite = true;
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(name, "relative-path") == 0)
    {
        strcpy(content, "installed/sample.exe");
        rewrite = true;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(name, "outside") == 0)
    {
        strcpy(content, fixture.manifest);
        rewrite = true;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(name, "prefix-sibling") == 0)
    {
        snprintf(content, sizeof(content), "%s/installed-extra/sample.exe", root);
        rewrite = true;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(name, "prefix-itself") == 0)
    {
        snprintf(content, sizeof(content), "%s/installed", root);
        rewrite = true;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(name, "traversal") == 0)
    {
        snprintf(content, sizeof(content), "%s/installed/../outside.exe", root);
        rewrite = true;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(name, "duplicate") == 0)
    {
        snprintf(content, sizeof(content), "%s\n%s\n", fixture.program, fixture.program);
        rewrite = true;
        expected = UMI_STATUS_ALREADY_EXISTS;
    }
    if (strcmp(name, "invalid-utf8") == 0)
    {
        snprintf(content, sizeof(content), "%s/installed/\xc0\xaf.exe", root);
        rewrite = true;
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(name, "control") == 0)
    {
        snprintf(content, sizeof(content), "%s/installed/\t.exe", root);
        rewrite = true;
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (rewrite)
        CHECK(umi_fs_write_text(fixture.manifest, content) == UMI_STATUS_OK);
    if (strcmp(name, "embedded-nul") == 0)
    {
        const unsigned char bad[] = {'a', 0, 'b'};
        CHECK(umi_fs_write_bytes(fixture.manifest, bad, sizeof(bad)) == UMI_STATUS_OK);
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(name, "missing-manifest") == 0)
    {
        CHECK(umi_fs_remove_tree(fixture.manifest) == UMI_STATUS_OK);
        expected = UMI_STATUS_NOT_FOUND;
    }
    if (strcmp(name, "relative-source") == 0)
    {
        strcpy(fixture.profile.source_directory, ".");
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(name, "empty-build") == 0)
    {
        fixture.profile.build_directory[0] = '\0';
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(name, "empty-install") == 0)
    {
        fixture.profile.install_directory[0] = '\0';
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(name, "missing-entry") == 0)
        CHECK(umi_fs_remove_tree(fixture.program) == UMI_STATUS_OK);
    if (strcmp(name, "directory-entry") == 0)
    {
        CHECK(umi_fs_remove_tree(fixture.program) == UMI_STATUS_OK);
        CHECK(umi_fs_make_directories(fixture.program) == UMI_STATUS_OK);
    }
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    if (strcmp(name, "cancelled") == 0)
    {
        umi_cancellation_token_request(cancel);
        expected = UMI_STATUS_CANCELLED;
    }
    if (strcmp(name, "cancel-reset") == 0)
    {
        umi_cancellation_token_request(cancel);
        umi_cancellation_token_reset(cancel);
    }
    if (strcmp(name, "entry-limit") == 0)
    {
        size_t line = strlen(fixture.program) + 1U;
        size_t size = line * (UMI_PROJECT_INSTALLED_FILE_LIMIT + 1U);
        char *many = malloc(size);
        CHECK(many != NULL);
        for (size_t i = 0U; i < UMI_PROJECT_INSTALLED_FILE_LIMIT + 1U; ++i)
        {
            memcpy(many + i * line, fixture.program, line - 1U);
            many[i * line + line - 1U] = '\n';
        }
        UmiStatus written = umi_fs_write_bytes(fixture.manifest, many, size);
        free(many);
        CHECK(written == UMI_STATUS_OK);
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    CHECK(UmiProjectInstalledFilesRead(&fixture.profile, cancel, &files) == expected);
    CHECK((files != NULL) == (expected == UMI_STATUS_OK));
    if (expected != UMI_STATUS_OK)
        goto cleanup;
    UmiProjectInstalledSummary summary;
    CHECK(UmiProjectInstalledFilesSummary(files, &summary) == UMI_STATUS_OK);
    CHECK(summary.count == (strcmp(name, "empty") == 0 ? 0U : strcmp(name, "no-newline") == 0 ? 1U : 2U));
    if (strcmp(name, "empty") == 0)
        goto cleanup;
    UmiProjectInstalledFile file;
    CHECK(UmiProjectInstalledFilesAt(files, 0U, &file) == UMI_STATUS_OK);
    CHECK(umi_path_equal(file.path, fixture.program));
    if (strcmp(name, "missing-entry") == 0)
    {
        CHECK(file.inspection_status == UMI_STATUS_NOT_FOUND && !file.program_candidate);
        goto cleanup;
    }
    if (strcmp(name, "directory-entry") == 0)
    {
        CHECK(file.kind == UMI_FILE_KIND_DIRECTORY && !file.program_candidate);
        goto cleanup;
    }
    CHECK(file.inspection_status == UMI_STATUS_OK && file.program_candidate &&
          file.kind == UMI_FILE_KIND_REGULAR);
    size_t index = 0U;
    expected = UMI_STATUS_OK;
    if (strcmp(name, "data-selection") == 0)
    {
        index = 1U;
        expected = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(name, "index") == 0)
    {
        index = summary.count;
        expected = UMI_STATUS_NOT_FOUND;
    }
    if (strcmp(name, "changed-root") == 0)
    {
        strcpy(fixture.profile.install_directory, "elsewhere");
        expected = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(name, "changed-build") == 0)
    {
        strcpy(fixture.profile.build_directory, "another-build");
        expected = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(name, "changed-manifest") == 0)
    {
        CHECK(umi_fs_write_text(fixture.manifest, "") == UMI_STATUS_OK);
        expected = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(name, "changed-file") == 0)
    {
        CHECK(umi_fs_write_text(fixture.program, "different length") == UMI_STATUS_OK);
        expected = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(name, "removed-file") == 0)
    {
        CHECK(umi_fs_remove_tree(fixture.program) == UMI_STATUS_OK);
        expected = UMI_STATUS_NOT_FOUND;
    }
    if (strcmp(name, "select-cancel") == 0)
    {
        umi_cancellation_token_request(cancel);
        expected = UMI_STATUS_CANCELLED;
    }
    if (strcmp(name, "permissions") == 0)
    {
#ifndef _WIN32
        CHECK(chmod(fixture.program, 0600) == 0);
        expected = UMI_STATUS_INVALID_STATE;
#endif
    }
    UmiBuildProfile selected = fixture.profile, before = selected;
    CHECK(UmiProjectInstalledFilesSelect(files, index, &fixture.profile, cancel, &selected) == expected);
    if (expected == UMI_STATUS_OK)
    {
        strcpy(before.run_program, file.path);
        CHECK(memcmp(&selected, &before, sizeof(before)) == 0);
        CHECK(UmiProjectInstalledFilesSelect(files, 0U, &fixture.profile, cancel, &fixture.profile) ==
              UMI_STATUS_OK);
        CHECK(memcmp(&fixture.profile, &before, sizeof(before)) == 0);
    }
    else
        CHECK(memcmp(&selected, &before, sizeof(before)) == 0);
cleanup:
    umi_cancellation_token_destroy(cancel);
    UmiProjectInstalledFilesDestroy(files);
    /* Ownership was acquired only after refusing any existing fixture path. */
    if (owned && umi_fs_remove_tree(root) != UMI_STATUS_OK)
        failed = 1;
    return failed;
}
