/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/single_query_attention.c
 * PURPOSE: A forward-only single-query CPU attention kernel for the existing
 *          bounded native Transformer family. No application-local engine.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/single_query_attention.h"
#include "umicom/ai/training/tape.h"

#include <float.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* Refuse ranges whose address arithmetic overflows. The public contract
 * requires valid accessible caller-owned memory even after this check. */
static int Overlaps(const void *left, size_t leftBytes,
                    const void *right, size_t rightBytes)
{
    const uintptr_t a = (uintptr_t)left;
    const uintptr_t b = (uintptr_t)right;
    if (a > UINTPTR_MAX - leftBytes || b > UINTPTR_MAX - rightBytes)
        return 1;
    return a < b + rightBytes && b < a + leftBytes;
}

UmiStatus UmiAiTrainingSingleQueryAttentionForward(
    const float *query, size_t queryCapacity,
    const float *keys, size_t keyCapacity,
    const float *values, size_t valueCapacity,
    size_t validRows, size_t channels, size_t headCount,
    float *outRow, size_t outCapacity,
    UmiAiTrainingSingleQueryAttentionReport *outReport)
{
    double *scores = NULL;
    float *probabilities = NULL;
    float *candidate = NULL;
    UmiAiTrainingSingleQueryAttentionReport report = {0};
    size_t kvElements, kvBytes, rowBytes;
    const size_t reportBytes = sizeof(report);
    UmiStatus status = UMI_STATUS_OK;

    if (query == NULL || keys == NULL || values == NULL || outRow == NULL ||
        validRows == 0U || channels == 0U || headCount == 0U ||
        channels % headCount != 0U ||
        validRows > UMI_AI_TRAINING_MAX_ELEMENTS / channels)
        return UMI_STATUS_INVALID_ARGUMENT;

    kvElements = validRows * channels;
    if (queryCapacity < channels || keyCapacity < kvElements ||
        valueCapacity < kvElements || outCapacity < channels)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (kvElements > SIZE_MAX / sizeof(float) ||
        channels > SIZE_MAX / sizeof(float) ||
        validRows > SIZE_MAX / sizeof(double) ||
        validRows > SIZE_MAX / sizeof(float))
        return UMI_STATUS_CAPACITY_EXCEEDED;

    kvBytes = kvElements * sizeof(float);
    rowBytes = channels * sizeof(float);
    if (Overlaps(outRow, rowBytes, query, rowBytes) ||
        Overlaps(outRow, rowBytes, keys, kvBytes) ||
        Overlaps(outRow, rowBytes, values, kvBytes))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (outReport != NULL &&
        (Overlaps(outReport, reportBytes, outRow, rowBytes) ||
         Overlaps(outReport, reportBytes, query, rowBytes) ||
         Overlaps(outReport, reportBytes, keys, kvBytes) ||
         Overlaps(outReport, reportBytes, values, kvBytes)))
        return UMI_STATUS_INVALID_ARGUMENT;

    for (size_t col = 0U; col < channels; ++col)
        if (!isfinite(query[col])) return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t element = 0U; element < kvElements; ++element)
        if (!isfinite(keys[element]) || !isfinite(values[element]))
            return UMI_STATUS_INVALID_ARGUMENT;

    scores = malloc(validRows * sizeof(*scores));
    probabilities = malloc(validRows * sizeof(*probabilities));
    candidate = malloc(rowBytes);
    if (scores == NULL || probabilities == NULL || candidate == NULL) {
        status = UMI_STATUS_OUT_OF_MEMORY;
        goto cleanup;
    }

    const size_t headWidth = channels / headCount;
    const double scale = sqrt((double)headWidth);
    for (size_t head = 0U; head < headCount; ++head) {
        const size_t firstChannel = head * headWidth;
        double maximum = -INFINITY;
        double denominator = 0.0;
        /* The canonical tape calculates all earlier query rows. Only its
         * final-row mathematics is required by autoregressive decoding. */
        for (size_t past = 0U; past < validRows; ++past) {
            double dot = 0.0;
            const size_t keyStart = past * channels + firstChannel;
            for (size_t local = 0U; local < headWidth; ++local)
                dot += (double)query[firstChannel + local] *
                       (double)keys[keyStart + local];
            scores[past] = dot / scale;
            if (!isfinite(scores[past])) {
                status = UMI_STATUS_INVALID_ARGUMENT;
                goto cleanup;
            }
            if (scores[past] > maximum) maximum = scores[past];
        }
        for (size_t past = 0U; past < validRows; ++past)
            denominator += exp(scores[past] - maximum);
        if (!isfinite(denominator) || denominator <= 0.0) {
            status = UMI_STATUS_INVALID_ARGUMENT;
            goto cleanup;
        }
        for (size_t past = 0U; past < validRows; ++past) {
            const double probability = exp(scores[past] - maximum) / denominator;
            if (!isfinite(probability)) {
                status = UMI_STATUS_INVALID_ARGUMENT;
                goto cleanup;
            }
            probabilities[past] = (float)probability;
        }
        /* Preserve the same float-probability / double-accumulation order as
         * UmiAiTrainingMultiHeadCausalAttention's last query row. */
        for (size_t local = 0U; local < headWidth; ++local) {
            const size_t col = firstChannel + local;
            double result = 0.0;
            for (size_t past = 0U; past < validRows; ++past)
                result += (double)probabilities[past] *
                          (double)values[past * channels + col];
            if (!isfinite(result) || result > (double)FLT_MAX ||
                result < -(double)FLT_MAX) {
                status = UMI_STATUS_INVALID_ARGUMENT;
                goto cleanup;
            }
            candidate[col] = (float)result;
        }
    }

    report.validRows = validRows;
    report.channels = channels;
    report.headCount = headCount;
    report.headWidth = headWidth;
    report.temporaryBytes = validRows * sizeof(*scores) +
        validRows * sizeof(*probabilities) + rowBytes;
    report.scoredKeyRows = (uint64_t)headCount * (uint64_t)validRows;
    report.dotProductTerms = (uint64_t)validRows * (uint64_t)channels;
    report.weightedValueTerms = (uint64_t)validRows * (uint64_t)channels;

    /* One publication point. Earlier rejections do not alter either output. */
    memcpy(outRow, candidate, rowBytes);
    if (outReport != NULL) *outReport = report;

cleanup:
    free(candidate);
    free(probabilities);
    free(scores);
    return status;
}
