/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_source_evidence.c
 * PURPOSE: Verify that training admission is bound to actual reviewed bytes,
 *          never just unchecked textual assertions or digest-shaped strings.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/source_evidence.h"
#include "umicom/base/sha256.h"

#include <stdio.h>
#include <string.h>

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "Source evidence check failed at line %d: %s\n", \
                __LINE__, #condition); \
        return 1; \
    } \
} while (0)

int main(void)
{
    static const unsigned char original[] = {0x42U, 0x49U, 0x54U,
                                               0x53U, 0x00U, 0xf1U};
    static const char derivative[] =
        "Synthetic teaching transcript\nNo identifiable participants.\n";
    static const char altered[] =
        "Synthetic teaching transcript\nDifferent contents.\n";
    char originalDigest[UMI_SHA256_HEX_CAPACITY] = {0};
    char preparedDigest[UMI_SHA256_HEX_CAPACITY] = {0};
    char tamperedDigest[UMI_SHA256_HEX_CAPACITY] = {0};
    UmiAiTrainingAdmittedSource result = {0}, sentinel = {0};
    UmiAiTrainingSourceReview review = {
        .relativeTextPath = "prepared/synthetic-transcript.txt",
        .originalAssetSha256 = originalDigest,
        .preparedTextSha256 = preparedDigest,
        .licenceId = "CC0-1.0",
        .rightsEvidenceId = "reviewed-synthetic-fixture",
        .medium = UMI_AI_TRAINING_SOURCE_VIDEO_TRANSCRIPT,
        .visibility = UMI_AI_TRAINING_SOURCE_VISIBILITY_PUBLIC,
        .rightsReviewed = 1,
        .trainingUseApproved = 1,
        .personalDataReviewed = 1,
        .extractedTextReviewed = 1
    };
    CHECK(UmiSha256Buffer(original, sizeof(original), originalDigest) == UMI_STATUS_OK);
    CHECK(UmiSha256Buffer(derivative, sizeof(derivative) - 1U,
        preparedDigest) == UMI_STATUS_OK);
    CHECK(UmiSha256Buffer(altered, sizeof(altered) - 1U,
        tamperedDigest) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingSourceVerifyBytes(&review, original, sizeof(original),
        derivative, sizeof(derivative) - 1U, 20U, &result) == UMI_STATUS_OK);
    CHECK(strcmp(result.relativeTextPath, review.relativeTextPath) == 0);
    CHECK(strcmp(result.preparedTextSha256, preparedDigest) == 0);
    CHECK(result.partition == UMI_AI_TRAINING_PARTITION_TRAIN ||
          result.partition == UMI_AI_TRAINING_PARTITION_EVALUATE);

    sentinel = result;
    review.preparedTextSha256 = tamperedDigest;
    CHECK(UmiAiTrainingSourceVerifyBytes(&review, original, sizeof(original),
        derivative, sizeof(derivative) - 1U, 20U, &result) == UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(&sentinel, &result, sizeof(result)) == 0);
    review.preparedTextSha256 = preparedDigest;
    CHECK(UmiAiTrainingSourceVerifyBytes(&review, original, sizeof(original),
        altered, sizeof(altered) - 1U, 20U, &result) == UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(&sentinel, &result, sizeof(result)) == 0);

    review.visibility = UMI_AI_TRAINING_SOURCE_VISIBILITY_PRIVATE;
    CHECK(UmiAiTrainingSourceVerifyBytes(&review, original, sizeof(original),
        derivative, sizeof(derivative) - 1U, 20U, &result) == UMI_STATUS_PERMISSION_DENIED);
    CHECK(memcmp(&sentinel, &result, sizeof(result)) == 0);
    review.visibility = UMI_AI_TRAINING_SOURCE_VISIBILITY_PUBLIC;
    review.extractedTextReviewed = 0;
    CHECK(UmiAiTrainingSourceVerifyBytes(&review, original, sizeof(original),
        derivative, sizeof(derivative) - 1U, 20U, &result) == UMI_STATUS_PERMISSION_DENIED);
    CHECK(memcmp(&sentinel, &result, sizeof(result)) == 0);
    review.extractedTextReviewed = 1;
    review.trainingUseApproved = 0;
    CHECK(UmiAiTrainingSourceVerifyBytes(&review, original, sizeof(original),
        derivative, sizeof(derivative) - 1U, 20U, &result) == UMI_STATUS_PERMISSION_DENIED);
    CHECK(memcmp(&sentinel, &result, sizeof(result)) == 0);
    review.trainingUseApproved = 1;
    CHECK(UmiAiTrainingSourceVerifyBytes(&review, original, sizeof(original),
        derivative, sizeof(derivative) - 1U, 0U, &result) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingSourceVerifyBytes(&review, original, sizeof(original),
        derivative, 0U, 20U, &result) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingSourceVerifyBytes(&review, NULL, sizeof(original),
        derivative, sizeof(derivative) - 1U, 20U, &result) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&sentinel, &result, sizeof(result)) == 0);

    puts("Verified source byte digests, privacy admission and atomic rejection.");
    return 0;
}
