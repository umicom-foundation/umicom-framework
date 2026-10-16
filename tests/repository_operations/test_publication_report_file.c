/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_operations/test_publication_report_file.c
 * PURPOSE: Protect source checkouts and earlier evidence when saving publication results.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/publication_report.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/clock.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(condition) do { if (!(condition)) { \
    (void)fprintf(stderr, "Report file check failed at line %d: %s\n", __LINE__, #condition); \
    goto finish; } } while (0)

int main(void)
{
    UmiRepositoryPublicationReport *report = calloc(1U, sizeof(*report));
    UmiOutputFile *file = NULL, *collision = NULL;
    char temp[UMI_PATH_CAPACITY], root[UMI_PATH_CAPACITY] = {0};
    char repository[UMI_PATH_CAPACITY], private_directory[UMI_PATH_CAPACITY];
    char marker[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY], leaf[96];
    char *text = NULL, *again = NULL;
    size_t size = 0U, again_size = 0U;
    int owns_fixture = 0, result = EXIT_FAILURE;
    UmiClock clock = umi_clock_system();
    if (!report) goto finish;
    CHECK(umi_fs_temp_directory(temp, sizeof(temp)) == UMI_STATUS_OK);
    (void)snprintf(leaf, sizeof(leaf), "umicom-publication-report-%llu",
        (unsigned long long)clock.monotonic_nanoseconds(&clock));
    CHECK(umi_path_join(temp, leaf, root, sizeof(root)) == UMI_STATUS_OK);
    CHECK(!umi_fs_exists(root));
    CHECK(umi_fs_make_directories(root) == UMI_STATUS_OK);
    owns_fixture = 1;
    CHECK(umi_path_join(root, "repo", repository, sizeof(repository)) == UMI_STATUS_OK);
    CHECK(umi_fs_make_directories(repository) == UMI_STATUS_OK);
    CHECK(umi_path_join(root, "repo-evidence", private_directory, sizeof(private_directory)) == UMI_STATUS_OK);
    CHECK(umi_fs_make_directories(private_directory) == UMI_STATUS_OK);
    CHECK(umi_path_join(repository, ".git", marker, sizeof(marker)) == UMI_STATUS_OK);
    CHECK(umi_fs_write_text(marker, "fixture metadata marker\n") == UMI_STATUS_OK);
    CHECK(umi_path_join(repository, "report.json", path, sizeof(path)) == UMI_STATUS_OK);
    /* Even a checkout absent from an incomplete discovery must not receive
     * the report. A .git file is as significant as a .git directory. */
    CHECK(umi_repository_publication_report_check_path(report, path) == UMI_STATUS_PERMISSION_DENIED);
    report->count = report->repository_count = 1U;
    report->entries[0].parent = SIZE_MAX;
    (void)snprintf(report->entries[0].path, sizeof(report->entries[0].path), "%s", repository);
    CHECK(umi_repository_publication_report_check_path(report, path) == UMI_STATUS_PERMISSION_DENIED);
    CHECK(!umi_fs_exists(path));
    CHECK(umi_repository_publication_report_check_path(report, "relative-report.json") != UMI_STATUS_OK);
    CHECK(umi_path_join(root, "missing/report.json", path, sizeof(path)) == UMI_STATUS_OK);
    CHECK(umi_repository_publication_report_check_path(report, path) == UMI_STATUS_NOT_FOUND);
    CHECK(umi_path_join(private_directory, "report.json", path, sizeof(path)) == UMI_STATUS_OK);
    CHECK(umi_repository_publication_report_check_path(report, path) == UMI_STATUS_OK);
    CHECK(UmiOutputFileCreate(path, &file) == UMI_STATUS_OK);
    /* Serialization validates the entire report before writing any bytes. */
    report->count = UMI_REPOSITORY_PUBLICATION_CAPACITY + 1U;
    CHECK(umi_repository_publication_report_write_json(report,
        UMI_REPOSITORY_PUBLICATION_REPORT_PREVIEW, file) == UMI_STATUS_INVALID_ARGUMENT);
    UmiOutputFileSnapshot empty_snapshot;
    CHECK(UmiOutputFileRead(file, &empty_snapshot) == UMI_STATUS_OK && empty_snapshot.bytes_written == 0U);
    report->count = 1U;
    CHECK(umi_repository_publication_report_write_json(report,
        UMI_REPOSITORY_PUBLICATION_REPORT_PREVIEW, file) == UMI_STATUS_OK);
    CHECK(UmiOutputFileClose(file) == UMI_STATUS_OK);
    UmiOutputFileSnapshot snapshot;
    CHECK(UmiOutputFileRead(file, &snapshot) == UMI_STATUS_OK && snapshot.closed && snapshot.bytes_written > 0U);
    CHECK(umi_fs_read_text(path, &text, &size) == UMI_STATUS_OK && text != NULL);
    CHECK(strstr(text, "\"publication_completed\":false") && strstr(text, "\"mode\":\"preview\""));
    CHECK(UmiOutputFileCreate(path, &collision) == UMI_STATUS_ALREADY_EXISTS && collision == NULL);
    CHECK(umi_repository_publication_report_write_json(report,
        UMI_REPOSITORY_PUBLICATION_REPORT_APPLY, file) != UMI_STATUS_OK);
    CHECK(umi_fs_read_text(path, &again, &again_size) == UMI_STATUS_OK);
    CHECK(size == again_size && memcmp(text, again, size) == 0);
    result = EXIT_SUCCESS;
finish:
    UmiOutputFileDestroy(collision);
    UmiOutputFileDestroy(file);
    free(again);
    free(text);
    free(report);
    /* The only removed path is the unique test-owned fixture created above. */
    if (owns_fixture && umi_fs_remove_tree(root) != UMI_STATUS_OK) result = EXIT_FAILURE;
    return result;
}
