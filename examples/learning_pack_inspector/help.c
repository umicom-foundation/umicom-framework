/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning_pack_inspector/help.c
 *
 * PURPOSE:
 *   Resolve an application help identity and verify its selected guide with the SDK.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/learning_help.h"
#include "umicom/teacher/learning_reader.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    if (argc != 5) {
        fprintf(stderr, "Usage: %s ABSOLUTE_PACK_ROOT MANIFEST command|diagnostic|topic KEY\n", argv[0]);
        return 2;
    }
    UmiLearningHelpRequest request = {.product = "studio", .locale = "en"};
    if (strcmp(argv[3], "command") == 0) request.kind = UMI_LEARNING_HELP_COMMAND;
    else if (strcmp(argv[3], "diagnostic") == 0) request.kind = UMI_LEARNING_HELP_DIAGNOSTIC;
    else if (strcmp(argv[3], "topic") == 0) request.kind = UMI_LEARNING_HELP_TOPIC;
    else return 2;
    size_t key_bytes = strlen(argv[4]);
    if (key_bytes >= sizeof(request.key)) return 2;
    memcpy(request.key, argv[4], key_bytes + 1U);
    /* Routing policy is supplied by the application. A document cannot turn
     * its title into an executable command or replace a different route. */
    static const UmiLearningHelpRoute routes[] = {
        {{"studio", "en", UMI_LEARNING_HELP_COMMAND, "help.documentation"},
            "framework.learning-packs", "guide"},
        {{"studio", "en", UMI_LEARNING_HELP_DIAGNOSTIC, "learning.content-changed"},
            "framework.learning-packs", "guide"},
        {{"studio", "en", UMI_LEARNING_HELP_TOPIC, "learning-packs"},
            "framework.learning-packs", "guide"}
    };
    UmiLearningPack *pack = NULL;
    UmiLearningShelf *shelf = NULL;
    UmiLearningHelpMap *map = NULL;
    UmiLearningReader *reader = NULL;
    UmiLearningHelpSelection selected;
    UmiLearningReaderSnapshot snapshot;
    UmiStatus status = UmiLearningPackLoad(argv[1], argv[2], (UmiVersion){0U,9U,0U}, &pack);
    if (status == UMI_STATUS_OK) status = UmiLearningShelfCreate(&shelf);
    if (status == UMI_STATUS_OK) status = UmiLearningShelfAdopt(shelf, &pack);
    if (status == UMI_STATUS_OK)
        status = UmiLearningHelpMapCreate(routes, sizeof(routes)/sizeof(routes[0]), &map);
    if (status == UMI_STATUS_OK) status = UmiLearningHelpResolve(map, shelf, &request, &selected);
    if (status == UMI_STATUS_OK)
        status = UmiLearningReaderCreateFromShelf(shelf, selected.pack.id, selected.pack.revision,
            selected.resource.id, selected.resource.sha256, 1024U * 1024U, &reader);
    if (status == UMI_STATUS_OK) status = UmiLearningReaderCopySnapshot(reader, &snapshot);
    if (status == UMI_STATUS_OK) {
        printf("Help: %s -> %s::%s\n", request.key, snapshot.pack.id, snapshot.selection.resource.id);
        printf("Verified bytes: %zu\n", snapshot.content_size);
    } else fprintf(stderr, "Help resolution or verified reading failed: %s\n", umi_status_text(status));
    UmiLearningReaderDestroy(reader);
    UmiLearningHelpMapDestroy(map);
    UmiLearningShelfDestroy(shelf);
    UmiLearningPackDestroy(pack);
    return status == UMI_STATUS_OK ? 0 : status == UMI_STATUS_NOT_FOUND ? 4 : 1;
}
