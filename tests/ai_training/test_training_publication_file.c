/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_publication_file.c
 * PURPOSE: Prove byte-exact, non-clobbering native training publication.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/training_publication.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(_WIN32)
#include <process.h>
#define UMI_TEST_PID _getpid
#else
#include <unistd.h>
#define UMI_TEST_PID getpid
#endif
#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); \
    return 1; } } while (0)

static int WriteFile(const char *path, const char *text)
{
    FILE *file = fopen(path, "wb");
    size_t length = strlen(text);
    if (file == NULL) return 0;
    int success = fwrite(text, 1U, length, file) == length;
    if (fclose(file) != 0) success = 0;
    return success;
}

static int FilePresent(const char *path)
{
    FILE *file = fopen(path, "rb");
    if (file == NULL) return 0;
    fclose(file);
    return 1;
}

int main(void)
{
    char base[128], clip[160], run[160], source[160], marker[160];
    UmiAiTrainingPublicationEvidence evidence;
    UmiAiTrainingPublicationEvidence saved;
    int formatted = snprintf(base, sizeof(base), "umi-test-publication-%ld.umi",
                             (long)UMI_TEST_PID());
    CHECK(formatted > 0 && (size_t)formatted < sizeof(base));
    CHECK(snprintf(clip, sizeof(clip), "%s.clip-policy", base) > 0);
    CHECK(snprintf(run, sizeof(run), "%s.training-lock", base) > 0);
    CHECK(snprintf(source, sizeof(source), "%s.source-lock", base) > 0);
    CHECK(snprintf(marker, sizeof(marker), "%s.publication", base) > 0);

    CHECK(WriteFile(base, "checkpoint bytes\n"));
    CHECK(WriteFile(clip, "policy bytes\n"));
    CHECK(WriteFile(run, "run lock bytes\n"));
    CHECK(WriteFile(source, "source bytes\n"));
    CHECK(UmiAiTrainingPublicationInspect(base, &evidence) == UMI_STATUS_OK);
    CHECK(evidence.checkpoint.byteCount == strlen("checkpoint bytes\n"));
    CHECK(strlen(evidence.checkpoint.sha256) == 64U);
    /* Independent published SHA-256 vectors: do not merely format and
     * verify a record using the same possible implementation mistake. */
    CHECK(strcmp(evidence.checkpoint.sha256,
          "5971668c84c3ab63515fdd54723e1b52bab0bfd3c757099a9abc3bb144a3e279") == 0);
    CHECK(strcmp(evidence.clipPolicy.sha256,
          "b321d1344ba9d315fbc20fe2731270df82c0a1d7592bfc2127bfc449284c1d9d") == 0);
    CHECK(strcmp(evidence.trainingLock.sha256,
          "070f1d2303ba067b44cd762d3c5f307b5bbcea74803bfe5055eaa175f24772d3") == 0);
    CHECK(strcmp(evidence.sourceLock.sha256,
          "8482cb75bd3c9abc864397324f7ba6cdbdb4920fe384ef7ee34b6ab4996ae8b2") == 0);
    CHECK(evidence.sourceLock.byteCount == strlen("source bytes\n"));
    CHECK(!FilePresent(marker));
    CHECK(UmiAiTrainingPublicationVerifyFile(base) == UMI_STATUS_IO_ERROR);
    CHECK(UmiAiTrainingPublicationSaveNewFile(base) == UMI_STATUS_OK);
    CHECK(FilePresent(marker));
    CHECK(UmiAiTrainingPublicationVerifyAndInspectFile(base, &evidence) == UMI_STATUS_OK);
    CHECK(evidence.checkpoint.byteCount == strlen("checkpoint bytes\n"));
    CHECK(UmiAiTrainingPublicationVerifyFile(base) == UMI_STATUS_OK);
    /* An occupied final marker is not updated or silently repaired. */
    CHECK(UmiAiTrainingPublicationSaveNewFile(base) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(UmiAiTrainingPublicationVerifyFile(base) == UMI_STATUS_OK);

    /* Change each individual byte source, then restore it in this disposable
     * test fixture. The marker must reject every changed file. */
    CHECK(WriteFile(base, "Checkpoint bytes\n"));
    saved = evidence;
    CHECK(UmiAiTrainingPublicationVerifyAndInspectFile(base, &evidence) ==
          UMI_STATUS_PARSE_ERROR);
    CHECK(memcmp(&evidence, &saved, sizeof(evidence)) == 0);
    CHECK(WriteFile(base, "checkpoint bytes\n"));
    CHECK(WriteFile(clip, "Policy bytes\n"));
    CHECK(UmiAiTrainingPublicationVerifyFile(base) == UMI_STATUS_PARSE_ERROR);
    CHECK(WriteFile(clip, "policy bytes\n"));
    CHECK(WriteFile(run, "Run lock bytes\n"));
    CHECK(UmiAiTrainingPublicationVerifyFile(base) == UMI_STATUS_PARSE_ERROR);
    CHECK(WriteFile(run, "run lock bytes\n"));
    CHECK(WriteFile(source, "Source bytes\n"));
    CHECK(UmiAiTrainingPublicationVerifyFile(base) == UMI_STATUS_PARSE_ERROR);
    CHECK(WriteFile(source, "source bytes\n"));
    CHECK(UmiAiTrainingPublicationVerifyFile(base) == UMI_STATUS_OK);

    /* A missing companion leaves all caller outputs unchanged. */
    saved = evidence;
    CHECK(remove(source) == 0);
    CHECK(UmiAiTrainingPublicationInspect(base, &evidence) == UMI_STATUS_IO_ERROR);
    CHECK(memcmp(&evidence, &saved, sizeof(evidence)) == 0);
    CHECK(UmiAiTrainingPublicationVerifyFile(base) == UMI_STATUS_IO_ERROR);
    CHECK(WriteFile(source, "source bytes\n"));
    CHECK(UmiAiTrainingPublicationVerifyFile(base) == UMI_STATUS_OK);
    CHECK(remove(base) == 0);
    CHECK(remove(clip) == 0);
    CHECK(remove(run) == 0);
    CHECK(remove(source) == 0);
    CHECK(remove(marker) == 0);
    puts("Training publication: checkpoint and three companion SHA-256 identities PASS");
    return 0;
}
