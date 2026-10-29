/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning_pack_inspector/history.c
 *
 * PURPOSE:
 *   Demonstrate verified lesson navigation and independent UI content copies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/learning_reader.h"
#include <stdio.h>
#include <stdlib.h>

static UmiStatus Describe(const UmiLearningReader *reader, const char *action)
{
    UmiLearningReaderSnapshot snapshot;
    UmiStatus status = UmiLearningReaderCopySnapshot(reader, &snapshot);
    if (status == UMI_STATUS_OK)
        (void)printf("%s: %s#%s, %zu bytes, back=%d, forward=%d\n", action,
            snapshot.selection.resource.id, snapshot.selection.fragment,
            snapshot.content_size, snapshot.can_go_back, snapshot.can_go_forward);
    return status;
}
int main(int argc, char **argv)
{
    if (argc != 5) {
        (void)fprintf(stderr,
            "Usage: umicom-learning-pack-history <absolute-root> <manifest> <first-id> <local-link>\n");
        return 2;
    }
    UmiLearningPack *pack = NULL;
    UmiLearningReader *reader = NULL;
    unsigned char *display_copy = NULL;
    size_t size = 0U;
    UmiVersion host = {UMICOM_FRAMEWORK_VERSION_MAJOR, UMICOM_FRAMEWORK_VERSION_MINOR,
        UMICOM_FRAMEWORK_VERSION_PATCH};
    UmiStatus status = UmiLearningPackLoad(argv[1], argv[2], host, &pack);
    if (status == UMI_STATUS_OK)
        status = UmiLearningReaderCreate(&pack, 4U * 1024U * 1024U, &reader);
    /* Adoption clears pack only on success. Both cleanup calls below therefore
     * remain correct whether loading, adoption or navigation fails. */
    if (status == UMI_STATUS_OK) status = UmiLearningReaderOpen(reader, argv[3]);
    if (status == UMI_STATUS_OK) status = Describe(reader, "Open");
    if (status == UMI_STATUS_OK) status = UmiLearningReaderFollow(reader, argv[4]);
    if (status == UMI_STATUS_OK) status = Describe(reader, "Follow");
    if (status == UMI_STATUS_OK) status = UmiLearningReaderBack(reader);
    if (status == UMI_STATUS_OK) status = Describe(reader, "Back");
    if (status == UMI_STATUS_OK) status = UmiLearningReaderForward(reader);
    if (status == UMI_STATUS_OK) status = Describe(reader, "Forward");
    if (status == UMI_STATUS_OK)
        status = UmiLearningReaderCopyContent(reader, NULL, 0U, &size);
    if (status == UMI_STATUS_OK) {
        display_copy = malloc(size + 1U);
        if (display_copy == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
    }
    if (status == UMI_STATUS_OK)
        status = UmiLearningReaderCopyContent(reader, display_copy, size + 1U, &size);
    UmiLearningReaderDestroy(reader);
    UmiLearningPackDestroy(pack);
    if (status == UMI_STATUS_OK) {
        /* An application can now transfer this independent allocation to its
         * UI. This console demonstration neither renders nor executes it. */
        (void)printf("Independent display copy: %zu bytes\n", size);
    } else {
        (void)fprintf(stderr, "Learning navigation failed: %s\n", umi_status_text(status));
    }
    free(display_copy);
    return status == UMI_STATUS_OK ? 0 : 1;
}
