/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_reviewed_corpus.c
 * PURPOSE: Verify reviewed JSONL bytes, source-held-out partitioning and
 *          duplicate detection with the existing SHA-256/admission owners.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/reviewed_corpus.h"

#include <stdio.h>
#include <string.h>

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "reviewed corpus line %d: %s\n", __LINE__, #condition); \
        return 1; \
    } \
} while (0)

int main(void)
{
    UmiAiTrainingReviewedExampleEvidence evidence[2];
    UmiAiTrainingReviewedCorpusReport summary = {0}, changed = {0};
    char records[2][512] = {{0}};
    const char *rows[2] = {records[0], records[1]};
    size_t lengths[2] = {0U, 0U};
    int haveTrain = 0, haveEval = 0;
    memset(evidence, 0, sizeof(evidence));

    /* Produce genuine admission evidence through the existing Framework
     * service, rather than asserting arbitrary editorial approval in a test. */
    for (unsigned candidate = 1U; candidate <= 64U && (!haveTrain || !haveEval);
         ++candidate) {
        char sourcePath[64], original[64], prepared[128];
        char originalDigest[65], preparedDigest[65];
        char json[512] = {0};
        size_t bytes = 0U;
        UmiAiTrainingReviewedExampleEvidence item = {0};
        int sourceLength = snprintf(sourcePath, sizeof(sourcePath),
                                    "training/example-%u.txt", candidate);
        int originalLength = snprintf(original, sizeof(original),
                                     "Synthetic public sample %u", candidate);
        int preparedLength = snprintf(prepared, sizeof(prepared),
                                     "Question %u\nAnswer %u", candidate, candidate);
        CHECK(sourceLength > 0 && originalLength > 0 && preparedLength > 0);
        CHECK(UmiSha256Buffer(original, (size_t)originalLength,
                              originalDigest) == UMI_STATUS_OK);
        CHECK(UmiSha256Buffer(prepared, (size_t)preparedLength,
                              preparedDigest) == UMI_STATUS_OK);
        const char *split = strchr(prepared, '\n');
        CHECK(split != NULL);
        size_t questionBytes = (size_t)(split - prepared);
        const UmiAiTrainingSourceReview review = {
            .relativeTextPath = sourcePath,
            .originalAssetSha256 = originalDigest,
            .preparedTextSha256 = preparedDigest,
            .licenceId = "MIT",
            .rightsEvidenceId = "fictional-test-fixture",
            .medium = UMI_AI_TRAINING_SOURCE_PLAIN_TEXT,
            .visibility = UMI_AI_TRAINING_SOURCE_VISIBILITY_PUBLIC,
            .rightsReviewed = 1,
            .trainingUseApproved = 1,
            .personalDataReviewed = 1,
            .extractedTextReviewed = 1
        };
        UmiStatus status = UmiAiTrainingReviewedExampleJsonl(&review,
            original, (size_t)originalLength,
            prepared, (size_t)preparedLength,
            (UmiAiTrainingTextSpan){0U, questionBytes},
            (UmiAiTrainingTextSpan){questionBytes + 1U,
                (size_t)preparedLength - questionBytes - 1U},
            50U, json, sizeof(json), &bytes, &item);
        CHECK(status == UMI_STATUS_OK);
        int index = -1;
        if (item.admittedSource.partition == UMI_AI_TRAINING_PARTITION_TRAIN &&
            !haveTrain) {
            haveTrain = 1;
            index = 0;
        } else if (item.admittedSource.partition == UMI_AI_TRAINING_PARTITION_EVALUATE &&
                   !haveEval) {
            haveEval = 1;
            index = 1;
        }
        if (index >= 0) {
            memcpy(records[index], json, bytes + 1U);
            lengths[index] = bytes;
            evidence[index] = item;
        }
    }
    CHECK(haveTrain && haveEval);
    CHECK(UmiAiTrainingReviewedCorpusAudit(rows, lengths, evidence,
                                            2U, &summary) == UMI_STATUS_OK);
    CHECK(summary.exampleCount == 2U && summary.trainingExamples == 1U &&
          summary.evaluationExamples == 1U && summary.distinctOriginalSources == 2U);
    CHECK(UmiAiTrainingReviewedCorpusAudit(rows, lengths, evidence,
                                            2U, &changed) == UMI_STATUS_OK);
    CHECK(memcmp(summary.orderedDigest, changed.orderedDigest,
                 sizeof(summary.orderedDigest)) == 0);

    char corrupted[512] = {0};
    memcpy(corrupted, records[0], lengths[0] + 1U);
    corrupted[12] = corrupted[12] == 'Q' ? 'X' : 'Q';
    const char *altered[2] = {corrupted, records[1]};
    memset(&changed, 0x55, sizeof(changed));
    UmiAiTrainingReviewedCorpusReport untouched = changed;
    CHECK(UmiAiTrainingReviewedCorpusAudit(altered, lengths, evidence,
                                            2U, &changed) == UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(&changed, &untouched, sizeof(changed)) == 0);

    UmiAiTrainingReviewedExampleEvidence badSplit[2] = {evidence[0], evidence[1]};
    strcpy(badSplit[1].admittedSource.originalAssetSha256,
           badSplit[0].admittedSource.originalAssetSha256);
    CHECK(UmiAiTrainingReviewedCorpusAudit(rows, lengths, badSplit,
                                            2U, &changed) == UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(&changed, &untouched, sizeof(changed)) == 0);

    const char *duplicate[2] = {records[0], records[0]};
    size_t duplicateSizes[2] = {lengths[0], lengths[0]};
    UmiAiTrainingReviewedExampleEvidence duplicateEvidence[2] = {evidence[0], evidence[0]};
    CHECK(UmiAiTrainingReviewedCorpusAudit(duplicate, duplicateSizes,
          duplicateEvidence, 2U, &changed) == UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(&changed, &untouched, sizeof(changed)) == 0);

    puts("Reviewed corpus: exact bytes, held-out source grouping and duplicates verified");
    return 0;
}
