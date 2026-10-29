/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning_pack_inspector/progress.c
 *
 * PURPOSE:
 *   Demonstrate explicit checkpoint records with temporary or selected SQLite storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/learning_progress.h"
#include "umicom/platform/path.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

static int Report(UmiStatus status)
{
    if (status == UMI_STATUS_OK) return 0;
    /* Use the public status-text contract; retain the earlier draft call for review. */
#if 0
    fprintf(stderr, "Learning progress: %s\n", umi_status_name(status));
#endif
    fprintf(stderr, "Learning progress: %s\n", umi_status_text(status));
    return status == UMI_STATUS_NOT_FOUND ? 4 : status == UMI_STATUS_ALREADY_EXISTS ? 3 : 1;
}
int main(int argc, char **argv)
{
    int memory = argc == 5 && strcmp(argv[1], "memory") == 0;
    int record = argc == 9 && strcmp(argv[1], "record") == 0;
    int show = argc == 8 && strcmp(argv[1], "show") == 0;
    if (!memory && !record && !show) {
        fprintf(stderr, "Usage:\n"
            "  learning-progress memory ROOT MANIFEST LESSON\n"
            "  learning-progress show ROOT MANIFEST LESSON DATABASE LEARNER CHECKPOINT\n"
            "  learning-progress record ROOT MANIFEST LESSON DATABASE LEARNER CHECKPOINT EVIDENCE\n");
        return 2;
    }
    if (!memory && !umi_path_is_absolute(argv[5])) {
        fprintf(stderr, "Choose an absolute local database path.\n");
        return 2;
    }
    UmiDataServer *server = NULL;
    UmiLearningProgress *progress = NULL;
    UmiLearningPack *pack = NULL;
    UmiLearningReader *reader = NULL;
    UmiLearningProgressEvent event;
    UmiStatus status = UmiLearningPackLoad(argv[2], argv[3], (UmiVersion){0U, 9U, 0U}, &pack);
    if (status == UMI_STATUS_OK) status = UmiLearningReaderCreate(&pack, 1024U * 1024U, &reader);
    if (status == UMI_STATUS_OK) status = UmiLearningReaderOpen(reader, argv[4]);
    /* Select storage only after the source document has been verified. The
     * read-only path refuses missing files instead of creating an empty record. */
    if (status == UMI_STATUS_OK) {
        if (memory) status = umi_data_server_create_memory(&server);
        else if (show) status = umi_data_server_open_sqlite_readonly(argv[5], &server);
        else status = umi_data_server_create_sqlite(argv[5], &server);
    }
    const char *learner = memory ? "example-learner" : argv[6];
    const char *checkpoint = memory ? "read" : argv[7];
    if (status == UMI_STATUS_OK) status = UmiLearningProgressOpen(server, learner, &progress);
    if (status == UMI_STATUS_OK && (memory || record))
        status = UmiLearningProgressRecord(progress, reader, checkpoint, memory ? "example-review" : argv[8], &event);
    if (status == UMI_STATUS_OK && memory) {
        /* Reopening the journal retains the borrowed in-memory Data Server.
         * It demonstrates ownership, not persistence across process shutdown. */
        UmiLearningProgressClose(progress);
        progress = NULL;
        status = UmiLearningProgressOpen(server, learner, &progress);
    }
    if (status == UMI_STATUS_OK)
        status = UmiLearningProgressFindCurrent(progress, reader, checkpoint, &event);
    if (status == UMI_STATUS_OK) {
        printf("Current checkpoint: %s\n", event.checkpoint);
        printf("Document: %s::%s revision %" PRIu64 "\n", event.pack_id, event.resource_id, event.pack_revision);
        printf("Event: %" PRIu64 "\n", event.sequence);
        printf("Storage: %s\n", memory ? "temporary" : "local SQLite");
    }
    UmiLearningProgressClose(progress);
    umi_data_server_destroy(server);
    UmiLearningReaderDestroy(reader);
    UmiLearningPackDestroy(pack);
    return Report(status);
}
