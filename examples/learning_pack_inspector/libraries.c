/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning_pack_inspector/libraries.c
 *
 * PURPOSE:
 *   Demonstrate copied library settings and fresh pack admission without automatic execution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/learning_library_choices.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s ABSOLUTE_PACK_FOLDER\n", argv[0]);
        return 2;
    }
    UmiLearningPack *pack = NULL, *reopened = NULL;
    UmiLearningLibraryChoices *choices = NULL, *decoded = NULL;
    char *encoded = NULL;
    size_t length = 0U;
    UmiStatus status = UmiLearningPackLoad(argv[1], "learning-pack.json",
        (UmiVersion){0,9,0}, &pack);
    if (status == UMI_STATUS_OK) status = UmiLearningLibraryChoicesCreate(&choices);
    if (status == UMI_STATUS_OK)
        status = UmiLearningLibraryChoicesRemember(choices, pack, "learning-pack.json");
    /* Remember copies the root and identity. The original pack can be released
     * before settings are encoded or a later session reopens the manifest. */
    UmiLearningPackDestroy(pack); pack = NULL;
    if (status == UMI_STATUS_OK) {
        status = UmiLearningLibraryChoicesEncode(choices, NULL, 0U, &length);
        if (status == UMI_STATUS_CAPACITY_EXCEEDED) {
            encoded = malloc(length + 1U);
            status = encoded == NULL ? UMI_STATUS_OUT_OF_MEMORY :
                UmiLearningLibraryChoicesEncode(choices, encoded, length + 1U, &length);
        }
    }
    if (status == UMI_STATUS_OK)
        status = UmiLearningLibraryChoicesDecode(encoded, length, &decoded);
    free(encoded); encoded = NULL;
    UmiLearningLibraryChoicesDestroy(choices); choices = NULL;
    /* Decode admits settings only. Reopen performs a fresh bounded manifest
     * read. A real application dispatches it to its file worker. Neither step
     * executes an example or searches another folder when the root is missing. */
    if (status == UMI_STATUS_OK)
        status = UmiLearningLibraryChoicesReopen(decoded, 0U, (UmiVersion){0,9,0}, &reopened);
    UmiLearningPackInfo info;
    if (status == UMI_STATUS_OK) status = UmiLearningPackCopyInfo(reopened, &info);
    if (status == UMI_STATUS_OK)
        printf("Remembered choices: %zu\nReopened: %s\n", UmiLearningLibraryChoicesCount(decoded), info.id);
    else fprintf(stderr, "Library choices: %s\n", umi_status_text(status));
    UmiLearningPackDestroy(reopened);
    UmiLearningLibraryChoicesDestroy(decoded);
    return status == UMI_STATUS_OK ? 0 : 1;
}
