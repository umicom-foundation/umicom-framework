/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning_pack_inspector/transaction.c
 *
 * PURPOSE:
 *   Demonstrate candidate navigation and an explicit accept or reject decision.
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
#include <string.h>

/* Each pointer below owns one object. A candidate is independent, so a failed
 * read or a rejected application decision cannot mutate the original reader. */
int main(int argc, char **argv)
{
    if (argc != 6 || (strcmp(argv[5], "accept") != 0 && strcmp(argv[5], "reject") != 0)) {
        fprintf(stderr, "Usage: learning-reader-transaction ROOT MANIFEST INITIAL_ID CANDIDATE_ID accept|reject\n");
        return 2;
    }
    UmiLearningPack *pack = NULL;
    UmiLearningReader *current = NULL, *candidate = NULL;
    UmiStatus status = UmiLearningPackLoad(argv[1], argv[2], (UmiVersion){0U, 9U, 0U}, &pack);
    if (status == UMI_STATUS_OK) status = UmiLearningReaderCreate(&pack, 1024U * 1024U, &current);
    if (status == UMI_STATUS_OK) status = UmiLearningReaderOpen(current, argv[3]);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Initial document: %s\n", umi_status_text(status));
        UmiLearningPackDestroy(pack);
        UmiLearningReaderDestroy(current);
        return 1;
    }
    status = UmiLearningReaderClone(current, &candidate);
    if (status == UMI_STATUS_OK) status = UmiLearningReaderOpen(candidate, argv[4]);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Candidate refused: %s\n", umi_status_text(status));
        puts("Decision: refused");
    } else if (strcmp(argv[5], "accept") == 0) {
        /* A graphical client prepares its presentation before this point.
         * This example's policy is the explicit command-line decision. */
        UmiLearningReaderDestroy(current);
        current = candidate;
        candidate = NULL;
        puts("Decision: accepted");
    } else puts("Decision: rejected");
    UmiLearningReaderSnapshot snapshot;
    UmiStatus observed = UmiLearningReaderCopySnapshot(current, &snapshot);
    if (observed == UMI_STATUS_OK)
        printf("Resource: %s\nHistory entries: %zu\n", snapshot.selection.resource.id, snapshot.history_count);
    else fprintf(stderr, "Reader snapshot: %s\n", umi_status_text(observed));
    UmiLearningReaderDestroy(candidate);
    UmiLearningReaderDestroy(current);
    UmiLearningPackDestroy(pack);
    return status == UMI_STATUS_OK && observed == UMI_STATUS_OK ? 0 : 1;
}
