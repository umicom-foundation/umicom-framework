/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_publication_rejections.c
 * PURPOSE: Exercise incomplete, oversized and malformed publication records.
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
static int Write(const char *path, const void *data, size_t size)
{
    FILE *file = fopen(path, "wb");
    if (file == NULL) return 0;
    int ok = fwrite(data, 1U, size, file) == size;
    if (fclose(file) != 0) ok = 0;
    return ok;
}
int main(void)
{
    char base[128], clip[160], run[160], source[160], marker[160], stage[160];
    UmiAiTrainingPublicationEvidence evidence;
    UmiAiTrainingPublicationEvidence saved;
    char tooLong[UMI_AI_TRAINING_PUBLICATION_MAX_PATH + 4U];
    char oversized[UMI_AI_TRAINING_PUBLICATION_MAX_COMPANION_BYTES + 1U];
    int formatted = snprintf(base, sizeof(base), "umi-test-rejections-%ld.umi",
                             (long)UMI_TEST_PID());
    CHECK(formatted > 0 && (size_t)formatted < sizeof(base));
    CHECK(snprintf(clip, sizeof(clip), "%s.clip-policy", base) > 0);
    CHECK(snprintf(run, sizeof(run), "%s.training-lock", base) > 0);
    CHECK(snprintf(source, sizeof(source), "%s.source-lock", base) > 0);
    CHECK(snprintf(marker, sizeof(marker), "%s.publication", base) > 0);
    CHECK(snprintf(stage, sizeof(stage), "%s.publication.partial", base) > 0);
    memset(&evidence, 0x63, sizeof(evidence));
    saved = evidence;
    CHECK(UmiAiTrainingPublicationInspect(NULL, &evidence) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingPublicationSaveNewFile(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingPublicationSaveNewFile("") == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingPublicationVerifyFile(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    memset(tooLong, 'x', sizeof(tooLong));
    tooLong[sizeof(tooLong) - 1U] = '\0';
    CHECK(UmiAiTrainingPublicationInspect(tooLong, &evidence) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(&evidence, &saved, sizeof(evidence)) == 0);
    CHECK(UmiAiTrainingPublicationSaveNewFile(base) == UMI_STATUS_IO_ERROR);
    CHECK(Write(base, "checkpoint", 10U));
    CHECK(Write(clip, "clip", 4U));
    CHECK(Write(run, "run", 3U));
    CHECK(UmiAiTrainingPublicationSaveNewFile(base) == UMI_STATUS_IO_ERROR);
    CHECK(Write(source, "source", 6U));
    /* Incomplete staging must not be overwritten and must not publish a marker. */
    CHECK(Write(stage, "partial marker", 14U));
    CHECK(UmiAiTrainingPublicationSaveNewFile(base) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(remove(stage) == 0);
    CHECK(UmiAiTrainingPublicationSaveNewFile(base) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingPublicationVerifyFile(base) == UMI_STATUS_OK);
    /* Noncanonical record is rejected even when all four evidence files exist. */
    CHECK(Write(marker, "unreviewed", 10U));
    CHECK(UmiAiTrainingPublicationVerifyFile(base) == UMI_STATUS_PARSE_ERROR);
    /* Refuse oversized companions without writing outputs. */
    memset(oversized, 'Q', sizeof(oversized));
    CHECK(Write(clip, oversized, sizeof(oversized)));
    CHECK(UmiAiTrainingPublicationInspect(base, &evidence) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(&evidence, &saved, sizeof(evidence)) == 0);
    CHECK(UmiAiTrainingPublicationVerifyFile(base) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(remove(base) == 0);
    CHECK(remove(clip) == 0);
    CHECK(remove(run) == 0);
    CHECK(remove(source) == 0);
    CHECK(remove(marker) == 0);
    puts("Training publication: staged/no-clobber/capacity/atomic-output rejections PASS");
    return 0;
}
