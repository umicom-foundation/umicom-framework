/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/source_admission.c
 * PURPOSE:
 *   Reject unreviewed, private and unlicensed training inputs before they
 *   become paths in the existing native dataset-manifest format.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/source_admission.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

static int AdmissionHexValue(char character)
{
    if (character >= '0' && character <= '9') return character - '0';
    if (character >= 'a' && character <= 'f') return character - 'a' + 10;
    if (character >= 'A' && character <= 'F') return character - 'A' + 10;
    return -1;
}

static int AdmissionDigestValid(const char *hex)
{
    if (hex == NULL) return 0;
    for (size_t index = 0U; index < 64U; ++index) {
        if (hex[index] == '\0' || AdmissionHexValue(hex[index]) < 0)
            return 0;
    }
    return hex[64] == '\0';
}

static int AdmissionTokenValid(const char *text)
{
    size_t length = 0U;
    if (text == NULL || text[0] == '\0') return 0;
    for (const unsigned char *p = (const unsigned char *)text;
         *p != '\0'; ++p) {
        unsigned char c = *p;
        if (++length > 96U ||
            !((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
              (c >= '0' && c <= '9') || c == '.' || c == '-' || c == '_' ||
              c == '+')) return 0;
    }
    return 1;
}

static int AdmissionPathValid(const char *path)
{
    size_t length = 0U;
    size_t component = 0U;
    if (path == NULL || path[0] == '\0') return 0;
    for (const unsigned char *p = (const unsigned char *)path;
         *p != '\0'; ++p) {
        unsigned char c = *p;
        if (++length >= UMI_AI_DATASET_MAX_PATH) return 0;
        if (c == '/') {
            if (component == 0U) return 0;
            if (component == 1U && p[-1] == '.') return 0;
            if (component == 2U && p[-2] == '.' && p[-1] == '.') return 0;
            component = 0U;
        } else if ((c >= 'a' && c <= 'z') ||
                   (c >= 'A' && c <= 'Z') ||
                   (c >= '0' && c <= '9') || c == '_' ||
                   c == '-' || c == '.') {
            ++component;
        } else {
            return 0;
        }
    }
    if (component == 0U) return 0;
    if (component == 1U && path[length - 1U] == '.') return 0;
    if (component == 2U && path[length - 2U] == '.' &&
        path[length - 1U] == '.') return 0;
    /* UTF-8 text derivatives use .txt; approved Markdown and native source
     * may remain in their original text-based formats. Binary documents,
     * media and arbitrary executable extensions cannot enter directly. */
    return (length >= 4U && strcmp(path + length - 4U, ".txt") == 0) ||
           (length >= 3U && strcmp(path + length - 3U, ".md") == 0) ||
           (length >= 2U && strcmp(path + length - 2U, ".c") == 0) ||
           (length >= 2U && strcmp(path + length - 2U, ".h") == 0);
}

static int AdmissionMediumValid(UmiAiTrainingSourceMedium medium)
{
    return medium >= UMI_AI_TRAINING_SOURCE_MARKDOWN &&
           medium <= UMI_AI_TRAINING_SOURCE_SOURCE_CODE;
}

UmiStatus UmiAiTrainingSourceAdmit(
    const UmiAiTrainingSourceReview *review,
    unsigned evaluationPercentage,
    UmiAiTrainingAdmittedSource *outSource)
{
    UmiAiTrainingAdmittedSource candidate = {0};
    uint64_t sourcePrefix = UINT64_C(0);

    if (review == NULL || outSource == NULL ||
        evaluationPercentage == 0U || evaluationPercentage > 50U ||
        !AdmissionMediumValid(review->medium) ||
        !AdmissionPathValid(review->relativeTextPath) ||
        !AdmissionDigestValid(review->originalAssetSha256) ||
        !AdmissionDigestValid(review->preparedTextSha256) ||
        !AdmissionTokenValid(review->licenceId) ||
        !AdmissionTokenValid(review->rightsEvidenceId))
        return UMI_STATUS_INVALID_ARGUMENT;

    if (review->visibility != UMI_AI_TRAINING_SOURCE_VISIBILITY_PUBLIC ||
        review->rightsReviewed != 1 ||
        review->trainingUseApproved != 1 ||
        review->personalDataReviewed != 1 ||
        (review->medium != UMI_AI_TRAINING_SOURCE_MARKDOWN &&
         review->medium != UMI_AI_TRAINING_SOURCE_PLAIN_TEXT &&
         review->medium != UMI_AI_TRAINING_SOURCE_SOURCE_CODE &&
         review->extractedTextReviewed != 1))
        return UMI_STATUS_PERMISSION_DENIED;

    /* Partition by ORIGINAL asset, not by individual extracted chunks. This
     * prevents copies/transcripts of the SAME SHA-256-identical source being
     * divided between train and evaluation. Near-duplicates still need review.
     * Only the digest prefix is used as a reproducible bucket; this is not a
     * source-authentication or cryptographic decision. */
    for (size_t index = 0U; index < 16U; ++index)
        sourcePrefix = (sourcePrefix << 4U) |
            (uint64_t)AdmissionHexValue(review->originalAssetSha256[index]);

    (void)strcpy(candidate.relativeTextPath, review->relativeTextPath);
    (void)strcpy(candidate.originalAssetSha256, review->originalAssetSha256);
    (void)strcpy(candidate.preparedTextSha256, review->preparedTextSha256);
    candidate.medium = review->medium;
    candidate.partition = (sourcePrefix % UINT64_C(100) <
        (uint64_t)evaluationPercentage)
        ? UMI_AI_TRAINING_PARTITION_EVALUATE
        : UMI_AI_TRAINING_PARTITION_TRAIN;
    *outSource = candidate;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingSourceManifestLine(
    const UmiAiTrainingAdmittedSource *source,
    char *outLine, size_t capacity)
{
    char candidate[UMI_AI_SOURCE_MANIFEST_LINE_CAPACITY] = {0};
    const char *label;
    int size;
    if (source == NULL || outLine == NULL ||
        !AdmissionPathValid(source->relativeTextPath) ||
        !AdmissionDigestValid(source->originalAssetSha256) ||
        !AdmissionDigestValid(source->preparedTextSha256))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (source->partition == UMI_AI_TRAINING_PARTITION_TRAIN)
        label = "train";
    else if (source->partition == UMI_AI_TRAINING_PARTITION_EVALUATE)
        label = "eval";
    else
        return UMI_STATUS_INVALID_ARGUMENT;
    size = snprintf(candidate, sizeof(candidate), "%s %s\n",
                    label, source->relativeTextPath);
    if (size < 0 || (size_t)size >= sizeof(candidate))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if ((size_t)size + 1U > capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(outLine, candidate, (size_t)size + 1U);
    return UMI_STATUS_OK;
}
