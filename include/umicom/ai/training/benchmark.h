/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/benchmark.h
 *
 * PURPOSE:
 *   Evaluate an approved native decoder on independently reviewed byte
 *   completion cases. Separate exact byte agreement from broader C23 skill.
 *
 * ARCHITECTURE AND OWNERSHIP:
 *   Decoder training, checkpoint loading, byte-tokenisation and sampling remain
 *   authoritative Framework services. Cases borrow caller-owned buffers;
 *   the caller retains the model and all output storage. The benchmark never
 *   interprets or executes generated source, opens files or updates weights.
 *   The caller must serialise access to the decoder while evaluating.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_BENCHMARK_H
#define UMICOM_AI_TRAINING_BENCHMARK_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/generation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_BENCHMARK_MAX_CASES 64U

typedef struct UmiAiTrainingBenchmarkCase {
    /* An optional display label; never used as a path or executed command. */
    const char *title;
    const uint8_t *promptBytes;
    size_t promptLength;
    const uint8_t *expectedBytes;
    size_t expectedLength;
} UmiAiTrainingBenchmarkCase;

typedef struct UmiAiTrainingBenchmarkCaseResult {
    size_t expectedBytes;
    size_t generatedBytes;
    size_t matchingPositionBytes;
    size_t matchingPrefixBytes;
    int exactMatch;
} UmiAiTrainingBenchmarkCaseResult;

typedef struct UmiAiTrainingBenchmarkReport {
    size_t caseCount;
    size_t expectedBytes;
    size_t generatedBytes;
    size_t matchingPositionBytes;
    size_t matchingPrefixBytes;
    size_t exactMatchCases;
    uint64_t completedOptimiserSteps;
} UmiAiTrainingBenchmarkReport;

/* Pure byte-level scoring. Exact match requires equal lengths and identical
 * bytes; position agreement compares only positions present in both arrays.
 * Matching prefix stops at the first mismatch. Empty strings are allowed for
 * this primitive. On failure, *outResult remains unchanged. */
UmiStatus UmiAiTrainingBenchmarkScoreBytes(
    const uint8_t *expectedBytes, size_t expectedLength,
    const uint8_t *generatedBytes, size_t generatedLength,
    UmiAiTrainingBenchmarkCaseResult *outResult);

/* Review cases BEFORE invoking the trained decoder. The decoder must have the
 * canonical 260-token byte vocabulary. Every expected completion contains
 * 1..256 bytes; prompt length is 0..4096 bytes. A benchmark consists of
 * 1..64 cases, and output capacity counts RESULT RECORDS, not bytes.
 *
 * Generate exactly expectedLength bytes with the established deterministic
 * Framework sampler; compare them without executing or compiling them.
 * Case sampling seeds are seed+(zero-based case index), with uint64 wrap
 * semantics; identical model/checkpoint, cases and seeds reproduce results.
 * The model's optimiser steps and weights remain unchanged. All outputs are
 * failure-atomic, including per-case arrays and the overall report.
 *
 * WARNING: Exact byte completion is NOT a measure of compiling, safety,
 * functional correctness, human usefulness or general C23 proficiency. */
UmiStatus UmiAiTrainingBenchmarkRun(
    UmiAiTrainingDecoder *model,
    const UmiAiTrainingBenchmarkCase *cases, size_t caseCount,
    const UmiAiTrainingSamplingOptions *options, uint64_t seed,
    UmiAiTrainingBenchmarkCaseResult *outCases, size_t capacity,
    UmiAiTrainingBenchmarkReport *outReport);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_BENCHMARK_H */
