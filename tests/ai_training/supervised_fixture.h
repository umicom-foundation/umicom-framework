/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/supervised_fixture.h
 * PURPOSE:
 *   Generate fictional, explicitly licensed native training fixtures through
 *   existing Framework admission and JSONL services for deterministic tests.
 *   No real documents, account records or personal data are included.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TEST_SUPERVISED_FIXTURE_H
#define UMICOM_AI_TEST_SUPERVISED_FIXTURE_H

#include "umicom/ai/training/supervised_dataset.h"
#include <stdio.h>
#include <string.h>

enum { UMI_TEST_SUPERVISED_EXAMPLES = 4, UMI_TEST_SUPERVISED_JSON_CAP = 512 };
typedef struct UmiTestSupervisedFixture {
    char jsonl[UMI_TEST_SUPERVISED_EXAMPLES][UMI_TEST_SUPERVISED_JSON_CAP];
    const char *records[UMI_TEST_SUPERVISED_EXAMPLES];
    size_t lengths[UMI_TEST_SUPERVISED_EXAMPLES];
    UmiAiTrainingReviewedExampleEvidence evidence[UMI_TEST_SUPERVISED_EXAMPLES];
} UmiTestSupervisedFixture;

/* Select precisely two fictional source groups per split without guessing
 * their SHA-based assignment. All human-review flags are explicit only for
 * the wholly synthetic in-memory records constructed by this test fixture. */
static inline UmiStatus UmiTestSupervisedFixturePrepare(UmiTestSupervisedFixture *out)
{
    size_t trainCount = 0U, evalCount = 0U;
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out, 0, sizeof(*out));
    for (unsigned sequence = 1U; sequence <= 256U &&
         (trainCount < 2U || evalCount < 2U); ++sequence) {
        char path[80], original[90], prepared[100];
        char originalSha[65], preparedSha[65], json[UMI_TEST_SUPERVISED_JSON_CAP];
        UmiAiTrainingReviewedExampleEvidence evidence = {0};
        size_t size = 0U;
        int pathCount = snprintf(path, sizeof(path),
                                 "fictional/source-%u.txt", sequence);
        int originalCount = snprintf(original, sizeof(original),
                                 "Fictional approved teaching source %u", sequence);
        int preparedCount = snprintf(prepared, sizeof(prepared),
                                 "Q%u\nA%u", sequence, sequence);
        if (pathCount < 0 || pathCount >= (int)sizeof(path) ||
            originalCount < 0 || originalCount >= (int)sizeof(original) ||
            preparedCount < 0 || preparedCount >= (int)sizeof(prepared))
            return UMI_STATUS_INTERNAL_ERROR;
        UmiStatus status = UmiSha256Buffer(original, (size_t)originalCount, originalSha);
        if (status != UMI_STATUS_OK) return status;
        status = UmiSha256Buffer(prepared, (size_t)preparedCount, preparedSha);
        if (status != UMI_STATUS_OK) return status;
        const char *separator = strchr(prepared, '\n');
        if (separator == NULL) return UMI_STATUS_INTERNAL_ERROR;
        size_t promptLength = (size_t)(separator - prepared);
        const UmiAiTrainingSourceReview review = {
            .relativeTextPath = path,
            .originalAssetSha256 = originalSha,
            .preparedTextSha256 = preparedSha,
            .licenceId = "MIT",
            .rightsEvidenceId = "synthetic-teaching-fixture",
            .medium = UMI_AI_TRAINING_SOURCE_PLAIN_TEXT,
            .visibility = UMI_AI_TRAINING_SOURCE_VISIBILITY_PUBLIC,
            .rightsReviewed = 1,
            .trainingUseApproved = 1,
            .personalDataReviewed = 1,
            .extractedTextReviewed = 1
        };
        status = UmiAiTrainingReviewedExampleJsonl(
            &review, original, (size_t)originalCount,
            prepared, (size_t)preparedCount,
            (UmiAiTrainingTextSpan){0U, promptLength},
            (UmiAiTrainingTextSpan){promptLength + 1U,
                                    (size_t)preparedCount - promptLength - 1U},
            50U, json, sizeof(json), &size, &evidence);
        if (status != UMI_STATUS_OK) return status;
        size_t target;
        if (evidence.admittedSource.partition == UMI_AI_TRAINING_PARTITION_TRAIN &&
            trainCount < 2U) target = trainCount++;
        else if (evidence.admittedSource.partition == UMI_AI_TRAINING_PARTITION_EVALUATE &&
                 evalCount < 2U) target = 2U + evalCount++;
        else continue;
        memcpy(out->jsonl[target], json, size + 1U);
        out->records[target] = out->jsonl[target];
        out->lengths[target] = size;
        out->evidence[target] = evidence;
    }
    return (trainCount == 2U && evalCount == 2U)
           ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}

#endif
