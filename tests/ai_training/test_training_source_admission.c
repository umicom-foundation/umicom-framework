/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_source_admission.c
 * PURPOSE: Exercise public-content approval, stable source splitting,
 *          media derivative review and failure-atomic manifest formatting.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/source_admission.h"

#include <stdio.h>
#include <string.h>

#define CHECK(value) do { \
    if (!(value)) { \
        fprintf(stderr, "Admission assertion %d: %s\n", __LINE__, #value); \
        return 1; \
    } \
} while (0)

int main(void)
{
    const char zeroDigest[] =
        "0000000000000000000000000000000000000000000000000000000000000000";
    const char otherDigest[] =
        "0000000000000040000000000000000000000000000000000000000000000000";
    UmiAiTrainingSourceReview review = {
        .relativeTextPath = "reviewed/example.txt",
        .originalAssetSha256 = zeroDigest,
        .preparedTextSha256 = otherDigest,
        .licenceId = "CC-BY-4.0",
        .rightsEvidenceId = "licence-review",
        .medium = UMI_AI_TRAINING_SOURCE_MARKDOWN,
        .visibility = UMI_AI_TRAINING_SOURCE_VISIBILITY_PUBLIC,
        .rightsReviewed = 1,
        .trainingUseApproved = 1,
        .personalDataReviewed = 1,
        .extractedTextReviewed = 0
    };
    UmiAiTrainingAdmittedSource source = {0};
    UmiAiTrainingAdmittedSource other = {0};
    char line[UMI_AI_SOURCE_MANIFEST_LINE_CAPACITY] = {0};
    char unchanged[UMI_AI_SOURCE_MANIFEST_LINE_CAPACITY];

    CHECK(UmiAiTrainingSourceAdmit(&review, 20U, &source) == UMI_STATUS_OK);
    CHECK(source.partition == UMI_AI_TRAINING_PARTITION_EVALUATE);
    CHECK(UmiAiTrainingSourceManifestLine(&source, line, sizeof(line)) ==
        UMI_STATUS_OK);
    CHECK(strcmp(line, "eval reviewed/example.txt\n") == 0);

    /* Other derivatives of one original asset cannot cross splits. */
    review.relativeTextPath = "reviewed/alternative.txt";
    review.preparedTextSha256 = zeroDigest;
    review.medium = UMI_AI_TRAINING_SOURCE_VIDEO_TRANSCRIPT;
    review.extractedTextReviewed = 1;
    CHECK(UmiAiTrainingSourceAdmit(&review, 20U, &other) == UMI_STATUS_OK);
    CHECK(other.partition == source.partition);

    review.originalAssetSha256 = otherDigest;
    CHECK(UmiAiTrainingSourceAdmit(&review, 20U, &other) == UMI_STATUS_OK);
    CHECK(other.partition == UMI_AI_TRAINING_PARTITION_TRAIN);

    /* Failure must not erase or partially publish an earlier accepted record. */
    UmiAiTrainingAdmittedSource sentinel = other;
    review.visibility = UMI_AI_TRAINING_SOURCE_VISIBILITY_PRIVATE;
    CHECK(UmiAiTrainingSourceAdmit(&review, 20U, &other) ==
        UMI_STATUS_PERMISSION_DENIED);
    CHECK(memcmp(&other, &sentinel, sizeof(other)) == 0);
    review.visibility = UMI_AI_TRAINING_SOURCE_VISIBILITY_PUBLIC;
    review.extractedTextReviewed = 0;
    CHECK(UmiAiTrainingSourceAdmit(&review, 20U, &other) ==
        UMI_STATUS_PERMISSION_DENIED);
    review.extractedTextReviewed = 1;
    review.trainingUseApproved = 0;
    CHECK(UmiAiTrainingSourceAdmit(&review, 20U, &other) ==
        UMI_STATUS_PERMISSION_DENIED);
    review.trainingUseApproved = 1;
    review.relativeTextPath = "../secret.txt";
    CHECK(UmiAiTrainingSourceAdmit(&review, 20U, &other) ==
        UMI_STATUS_INVALID_ARGUMENT);
    review.relativeTextPath = "reviewed/unapproved.exe";
    CHECK(UmiAiTrainingSourceAdmit(&review, 20U, &other) ==
        UMI_STATUS_INVALID_ARGUMENT);
    review.relativeTextPath = "reviewed/alternative.txt";
    review.originalAssetSha256 = "not-a-real-sha256";
    CHECK(UmiAiTrainingSourceAdmit(&review, 20U, &other) ==
        UMI_STATUS_INVALID_ARGUMENT);

    memset(unchanged, '#', sizeof(unchanged));
    memcpy(line, unchanged, sizeof(line));
    CHECK(UmiAiTrainingSourceManifestLine(&source, line, 4U) ==
        UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(line, unchanged, sizeof(line)) == 0);

    puts("Approved source metadata and media-manifest admission: PASS");
    return 0;
}
