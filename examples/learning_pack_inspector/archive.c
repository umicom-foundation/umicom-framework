/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning_pack_inspector/archive.c
 *
 * PURPOSE:
 *   Export private progress to a new file and import only into an empty journal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/learning_progress.h"
#include "umicom/platform/rooted_files.h"
#include "umicom/platform/path.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    int exporting = argc == 6 && strcmp(argv[1], "export") == 0;
    int importing = argc == 6 && strcmp(argv[1], "import") == 0;
    if ((!exporting && !importing) || !umi_path_is_absolute(argv[2])) {
        fprintf(stderr, "Usage: learning-archive export|import ABSOLUTE_DATABASE LEARNER ABSOLUTE_ARCHIVE_ROOT RELATIVE_FILE\n");
        return 2;
    }
    UmiStatus status = UmiRootedFileValidatePath(argv[4], argv[5]);
    UmiDataServer *server = NULL;
    UmiLearningProgress *progress = NULL;
    unsigned char *input = NULL;
    char *output = NULL;
    size_t size = 0U;
    UmiRootedFileCreation created = {0};
    /* Read a bounded archive before opening a destination database. Parsing
     * follows once the matching learner owner exists; failed import may leave
     * a newly created empty database, but never replaces an existing journal. */
    if (status == UMI_STATUS_OK && importing)
        status = UmiRootedFileRead(argv[4], argv[5], UMI_LEARNING_PROGRESS_MAX_ARCHIVE_BYTES, &input, &size);
    if (status == UMI_STATUS_OK) {
        status = exporting ? umi_data_server_open_sqlite_readonly(argv[2], &server) :
            umi_data_server_create_sqlite(argv[2], &server);
    }
    if (status == UMI_STATUS_OK) status = UmiLearningProgressOpen(server, argv[3], &progress);
    if (status == UMI_STATUS_OK && exporting) {
        status = UmiLearningProgressExport(progress, NULL, 0U, &size);
        if (status == UMI_STATUS_CAPACITY_EXCEEDED && size <= UMI_LEARNING_PROGRESS_MAX_ARCHIVE_BYTES) {
            output = malloc(size + 1U);
            status = output == NULL ? UMI_STATUS_OUT_OF_MEMORY :
                UmiLearningProgressExport(progress, output, size + 1U, &size);
        }
        if (status == UMI_STATUS_OK)
            status = UmiRootedFileCreate(argv[4], argv[5], output, size, &created);
    } else if (status == UMI_STATUS_OK) {
        status = UmiLearningProgressImportEmpty(progress, input, size);
    }
    if (status == UMI_STATUS_OK) {
        UmiLearningProgressSnapshot snapshot;
        status = UmiLearningProgressCopySnapshot(progress, &snapshot);
        if (status == UMI_STATUS_OK)
            printf("%s %zu checkpoint events. Keep the archive private.\n",
                exporting ? "Exported" : "Imported", snapshot.event_count);
    } else {
        fprintf(stderr, "Learning archive: %s\n", umi_status_text(status));
        if (created.created)
            fprintf(stderr, "A partial new archive remains (%zu accepted bytes); inspect it before choosing a new filename.\n",
                created.bytes_written);
        if (importing && server != NULL)
            fprintf(stderr, "The selected database may remain. Inspect its state before another import; no journal was deliberately cleared.\n");
    }
    UmiLearningProgressClose(progress);
    umi_data_server_destroy(server);
    UmiRootedFileFree(input);
    free(output);
    return status == UMI_STATUS_OK ? 0 : 1;
}
