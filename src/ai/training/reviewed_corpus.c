/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/reviewed_corpus.c
 * PURPOSE:
 *   Check actual reviewed-example bytes and source-group split invariants
 *   without logging or publishing training content.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/reviewed_corpus.h"

#include <stdint.h>
#include <string.h>

static int CorpusDigestValid(const char *digest)
{
    static const char hex[] = "0123456789abcdef";
    if (digest == NULL) return 0;
    for (size_t i = 0U; i < 64U; ++i) {
        if (memchr(hex, (unsigned char)digest[i], sizeof(hex) - 1U) == NULL)
            return 0;
    }
    return digest[64] == '\0';
}

static UmiStatus CorpusShaAppend(UmiSha256 *digest, const void *bytes, size_t count)
{
    return UmiSha256Update(digest, bytes, count);
}

static UmiStatus CorpusAppendInteger(UmiSha256 *digest, uint64_t number)
{
    unsigned char encoded[8];
    for (size_t i = 0U; i < sizeof(encoded); ++i)
        encoded[i] = (unsigned char)(number >> (8U * i));
    return CorpusShaAppend(digest, encoded, sizeof(encoded));
}

UmiStatus UmiAiTrainingReviewedCorpusAudit(
    const char *const *jsonlRecords, const size_t *jsonlLengths,
    const UmiAiTrainingReviewedExampleEvidence *evidence,
    size_t recordCount, UmiAiTrainingReviewedCorpusReport *outReport)
{
    static const char domain[] = "umicom-reviewed-corpus\n";
    UmiAiTrainingReviewedCorpusReport candidate = {0};
    UmiSha256 digest;
    UmiStatus status;

    if (jsonlRecords == NULL || jsonlLengths == NULL || evidence == NULL ||
        outReport == NULL || recordCount < 2U ||
        recordCount > UMI_AI_REVIEWED_CORPUS_MAX_EXAMPLES)
        return UMI_STATUS_INVALID_ARGUMENT;

    UmiSha256Init(&digest);
    status = CorpusShaAppend(&digest, domain, sizeof(domain) - 1U);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < recordCount; ++i) {
        const UmiAiTrainingReviewedExampleEvidence *item = &evidence[i];
        const UmiAiTrainingAdmittedSource *source = &item->admittedSource;
        char observedSha[UMI_SHA256_HEX_CAPACITY];
        const size_t length = jsonlLengths[i];
        UmiAiTrainingPartition partition = source->partition;
        if (jsonlRecords[i] == NULL || length < 31U ||
            length != item->jsonBytes ||
            length > 12U * UMI_AI_REVIEWED_EXAMPLE_MAX_SPAN_BYTES + 256U ||
            source->relativeTextPath[0] == '\0' ||
            (partition != UMI_AI_TRAINING_PARTITION_TRAIN &&
             partition != UMI_AI_TRAINING_PARTITION_EVALUATE) ||
            !CorpusDigestValid(source->originalAssetSha256) ||
            !CorpusDigestValid(source->preparedTextSha256) ||
            !CorpusDigestValid(item->jsonSha256))
            return UMI_STATUS_INVALID_ARGUMENT;
        /* Records must be complete JSONL lines, not a concatenation of
         * several records or a NUL-terminated string with hidden suffix. */
        if (jsonlRecords[i][length - 1U] != '\n' ||
            memchr(jsonlRecords[i], '\n', length - 1U) != NULL ||
            memchr(jsonlRecords[i], '\0', length) != NULL ||
            memcmp(jsonlRecords[i], "{\"prompt\":\"", 11U) != 0 ||
            memcmp(jsonlRecords[i] + length - 3U, "\"}\n", 3U) != 0)
            return UMI_STATUS_PARSE_ERROR;
        status = UmiSha256Buffer(jsonlRecords[i], length, observedSha);
        if (status != UMI_STATUS_OK) return status;
        if (memcmp(observedSha, item->jsonSha256, sizeof(observedSha)) != 0)
            return UMI_STATUS_INVALID_STATE;

        for (size_t previous = 0U; previous < i; ++previous) {
            const UmiAiTrainingAdmittedSource *prior =
                &evidence[previous].admittedSource;
            if (strcmp(evidence[previous].jsonSha256, item->jsonSha256) == 0)
                return UMI_STATUS_INVALID_STATE; /* Exact record duplication. */
            if (strcmp(prior->originalAssetSha256,
                       source->originalAssetSha256) == 0) {
                if (prior->partition != partition)
                    return UMI_STATUS_INVALID_STATE;
            } else if (strcmp(prior->preparedTextSha256,
                              source->preparedTextSha256) == 0 &&
                       prior->partition != partition) {
                return UMI_STATUS_INVALID_STATE;
            }
        }
        int firstSource = 1;
        for (size_t previous = 0U; previous < i; ++previous) {
            if (strcmp(evidence[previous].admittedSource.originalAssetSha256,
                       source->originalAssetSha256) == 0) {
                firstSource = 0;
                break;
            }
        }
        if (firstSource) ++candidate.distinctOriginalSources;
        if (partition == UMI_AI_TRAINING_PARTITION_TRAIN)
            ++candidate.trainingExamples;
        else
            ++candidate.evaluationExamples;
        if (candidate.jsonBytes > SIZE_MAX - length)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        candidate.jsonBytes += length;
        /* Hash an unambiguous ordered sequence using fixed-width lengths,
         * then exact digests and split. No source names or text are logged. */
        status = CorpusAppendInteger(&digest, (uint64_t)length);
        if (status == UMI_STATUS_OK)
            status = CorpusShaAppend(&digest, observedSha, 64U);
        if (status == UMI_STATUS_OK)
            status = CorpusShaAppend(&digest, source->originalAssetSha256, 64U);
        if (status == UMI_STATUS_OK)
            status = CorpusShaAppend(&digest, source->preparedTextSha256, 64U);
        if (status == UMI_STATUS_OK)
            status = CorpusAppendInteger(&digest, (uint64_t)partition);
        if (status != UMI_STATUS_OK) return status;
    }
    if (candidate.trainingExamples == 0U || candidate.evaluationExamples == 0U)
        return UMI_STATUS_INVALID_STATE;
    candidate.exampleCount = recordCount;
    status = UmiSha256Final(&digest, candidate.orderedDigest);
    if (status == UMI_STATUS_OK) *outReport = candidate;
    return status;
}
