/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning_pack_inspector/practice.c
 *
 * PURPOSE:
 *   Preview an example and copy verified bytes only after an explicit request.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/learning_practice.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    if ((argc != 6 && argc != 7) || (argc == 7 && strcmp(argv[6], "--copy") != 0)) {
        fprintf(stderr, "Usage: umicom-learning-pack-practice <absolute-pack-root> <manifest> "
            "<resource-id> <absolute-practice-root> <new-relative-file> [--copy]\n");
        return 2;
    }
    UmiVersion host = {UMICOM_FRAMEWORK_VERSION_MAJOR, UMICOM_FRAMEWORK_VERSION_MINOR,
        UMICOM_FRAMEWORK_VERSION_PATCH};
    UmiLearningPack *pack = NULL;
    UmiStatus status = UmiLearningPackLoad(argv[1], argv[2], host, &pack);
    UmiLearningPackResource example;
    if (status == UMI_STATUS_OK)
        status = UmiLearningPracticeResolveExample(pack, argv[3], &example);
    if (status == UMI_STATUS_OK)
        status = UmiRootedFileValidatePath(argv[4], argv[5]);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Cannot prepare practice copy: %s\n", umi_status_text(status));
        UmiLearningPackDestroy(pack);
        return 1;
    }
    printf("Example: %s (%s)\nDestination root: %s\nNew file: %s\n",
        example.title, example.id, argv[4], argv[5]);
    if (argc == 6) {
        puts("Preview only. No destination was opened. Add --copy to request creation.");
        puts("Preview checks metadata and path syntax; copy verifies the source bytes.");
        UmiLearningPackDestroy(pack);
        return 0;
    }
    UmiLearningPracticeResult result;
    status = UmiLearningPracticeCopy(pack, argv[3], argv[4], argv[5], 4U * 1024U * 1024U, &result);
    UmiLearningPackDestroy(pack);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Practice copy failed: %s\n", umi_status_text(status));
        if (result.destination_attempted && result.destination.created)
            fprintf(stderr, "A new partial file remains (%zu accepted bytes). Inspect it; "
                "no file was deleted and no retry will overwrite it.\n", result.destination.bytes_written);
        return 1;
    }
    printf("Copied %zu verified bytes. The original learning resource is unchanged.\n", result.source_size);
    puts("No project was opened, compiled, trusted or executed.");
    return 0;
}
