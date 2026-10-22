/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/benchmark.c
 *
 * PURPOSE:
 *   Bounded, deterministic completion benchmarking for the native decoder.
 *   No model mathematics or tokeniser implementation is duplicated here.
 *
 * FAILURE CONTRACT:
 *   Validate all cases, capacities and sampling options before generation.
 *   Stage every score and aggregate on the stack and publish only after the
 *   last case succeeds. This also protects results after numerical failures.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/benchmark.h"
#include "umicom/ai/training/byte_tokenizer.h"

#include <math.h>
#include <stdbool.h>
#include <string.h>

UmiStatus UmiAiTrainingBenchmarkScoreBytes(
    const uint8_t *expectedBytes, size_t expectedLength,
    const uint8_t *generatedBytes, size_t generatedLength,
    UmiAiTrainingBenchmarkCaseResult *outResult)
{
    size_t matching = 0U, prefix = 0U;
    const size_t minimum = expectedLength < generatedLength ?
                           expectedLength : generatedLength;
    UmiAiTrainingBenchmarkCaseResult candidate = {0};
    if (outResult == NULL ||
        (expectedLength > 0U && expectedBytes == NULL) ||
        (generatedLength > 0U && generatedBytes == NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t position = 0U; position < minimum; ++position) {
        if (expectedBytes[position] == generatedBytes[position]) {
            ++matching;
            if (prefix == position) ++prefix;
        }
    }
    candidate.expectedBytes = expectedLength;
    candidate.generatedBytes = generatedLength;
    candidate.matchingPositionBytes = matching;
    candidate.matchingPrefixBytes = prefix;
    candidate.exactMatch = expectedLength == generatedLength &&
                           matching == expectedLength;
    *outResult = candidate;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingBenchmarkRun(
    UmiAiTrainingDecoder *model,
    const UmiAiTrainingBenchmarkCase *cases, size_t caseCount,
    const UmiAiTrainingSamplingOptions *options, uint64_t seed,
    UmiAiTrainingBenchmarkCaseResult *outCases, size_t capacity,
    UmiAiTrainingBenchmarkReport *outReport)
{
    UmiAiTrainingBenchmarkCaseResult staged[UMI_AI_BENCHMARK_MAX_CASES];
    UmiAiTrainingBenchmarkReport report = {0};
    UmiAiTrainingDecoderConfig config;
    uint64_t completedSteps = 0U;
    uint8_t generated[UMI_AI_GENERATION_MAX_NEW_TOKENS];
    UmiStatus status;

    if (model == NULL || cases == NULL || options == NULL ||
        outCases == NULL || outReport == NULL ||
        caseCount == 0U || caseCount > UMI_AI_BENCHMARK_MAX_CASES)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (capacity < caseCount) return UMI_STATUS_CAPACITY_EXCEEDED;
    status = UmiAiTrainingDecoderInspect(model, &config, &completedSteps);
    if (status != UMI_STATUS_OK) return status;
    if (config.vocabularySize != UMI_AI_BYTE_VOCABULARY ||
        options->topK == 0U || options->topK > UMI_AI_BYTE_VOCABULARY - UMI_AI_BYTE_OFFSET ||
        !isfinite(options->temperature) || options->temperature <= 0.0F)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t index = 0U; index < caseCount; ++index) {
        const UmiAiTrainingBenchmarkCase *item = &cases[index];
        if ((item->promptLength > 0U && item->promptBytes == NULL) ||
            item->promptLength > UMI_AI_GENERATION_MAX_PROMPT_TOKENS ||
            item->expectedBytes == NULL || item->expectedLength == 0U ||
            item->expectedLength > UMI_AI_GENERATION_MAX_NEW_TOKENS)
            return UMI_STATUS_INVALID_ARGUMENT;
        /* Prevent aggregate arithmetic overflow even on 32-bit machines.
         * These bounds also guarantee each total is at most 64*256. */
        if (report.expectedBytes > SIZE_MAX - item->expectedLength)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        report.expectedBytes += item->expectedLength;
    }
    report.expectedBytes = 0U;
    report.caseCount = caseCount;
    report.completedOptimiserSteps = completedSteps;

    for (size_t index = 0U; index < caseCount; ++index) {
        const UmiAiTrainingBenchmarkCase *item = &cases[index];
        size_t written = 0U;
        status = UmiAiTrainingDecoderGenerateBytes(
            model, item->promptBytes, item->promptLength, options,
            item->expectedLength, seed + (uint64_t)index,
            generated, sizeof(generated), &written);
        if (status != UMI_STATUS_OK) return status;
        status = UmiAiTrainingBenchmarkScoreBytes(
            item->expectedBytes, item->expectedLength,
            generated, written, &staged[index]);
        if (status != UMI_STATUS_OK) return status;
        report.expectedBytes += staged[index].expectedBytes;
        report.generatedBytes += staged[index].generatedBytes;
        report.matchingPositionBytes += staged[index].matchingPositionBytes;
        report.matchingPrefixBytes += staged[index].matchingPrefixBytes;
        report.exactMatchCases += (size_t)staged[index].exactMatch;
    }
    memcpy(outCases, staged, caseCount * sizeof(*outCases));
    *outReport = report;
    return UMI_STATUS_OK;
}
