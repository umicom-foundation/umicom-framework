/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/benchmark_evidence.c
 *
 * PURPOSE:
 *   Produce portable, domain-separated SHA-256 identities and deterministic
 *   JSON for native completion benchmarks without claiming source correctness.
 *
 * DESIGN:
 *   Reuse existing decoder checkpoint and Framework base SHA-256 services.
 *   All counters are validated before hashing; checked formatting occurs in
 *   a private buffer before publication to make errors failure-atomic.
 *   The JSON is evidence of caller-reported metrics, NOT a cryptographically
 *   signed attestation that the benchmark was executed correctly.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/benchmark_evidence.h"
#include "umicom/ai/training/checkpoint.h"
#include "umicom/ai/training/byte_tokenizer.h"

#include <inttypes.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static UmiStatus HashInteger(UmiSha256 *hash, uint64_t number)
{
    unsigned char bytes[8];
    for (size_t i = 0U; i < 8U; ++i)
        bytes[i] = (unsigned char)(number >> (i * 8U));
    return UmiSha256Update(hash, bytes, sizeof(bytes));
}

static UmiStatus HashBytes(UmiSha256 *hash, const void *bytes, size_t length)
{
    UmiStatus status = HashInteger(hash, (uint64_t)length);
    if (status != UMI_STATUS_OK) return status;
    return UmiSha256Update(hash, bytes, length);
}

static UmiStatus BeginHash(UmiSha256 *hash, const char *domain)
{
    UmiSha256Init(hash);
    return HashBytes(hash, domain, strlen(domain));
}

static UmiStatus EndHash(UmiSha256 *hash, char outHex[UMI_SHA256_HEX_CAPACITY])
{
    unsigned char digest[UMI_SHA256_BYTES];
    UmiStatus status = UmiSha256Final(hash, digest);
    if (status == UMI_STATUS_OK) UmiSha256Hex(digest, outHex);
    return status;
}

/* Case names may originate in a reviewed source file or the application's
 * built-in examples. Bound them without invoking an unbounded strlen(). */
static bool CaseTitleLength(const char *title, size_t *outLength)
{
    size_t length = 0U;
    if (title == NULL || outLength == NULL) return false;
    while (length <= UMI_AI_BENCHMARK_EVIDENCE_MAX_TITLE_BYTES &&
           title[length] != '\0') {
        const unsigned char ch = (unsigned char)title[length];
        if (ch < 32U || ch > 126U) return false;
        ++length;
    }
    if (length == 0U || length > UMI_AI_BENCHMARK_EVIDENCE_MAX_TITLE_BYTES)
        return false;
    *outLength = length;
    return true;
}

static bool MatchesData(const UmiAiTrainingBenchmarkCase *cases,
                        const UmiAiTrainingBenchmarkCaseResult *results,
                        size_t count, const UmiAiTrainingBenchmarkReport *report)
{
    size_t expected = 0U, generated = 0U, positional = 0U, prefix = 0U;
    size_t exact = 0U;
    if (cases == NULL || results == NULL || report == NULL ||
        count == 0U || count > UMI_AI_BENCHMARK_MAX_CASES ||
        report->caseCount != count) return false;
    for (size_t i = 0U; i < count; ++i) {
        const UmiAiTrainingBenchmarkCase *item = &cases[i];
        const UmiAiTrainingBenchmarkCaseResult *score = &results[i];
        size_t titleLength;
        if (!CaseTitleLength(item->title, &titleLength) ||
            (item->promptLength != 0U && item->promptBytes == NULL) ||
            item->promptLength > UMI_AI_GENERATION_MAX_PROMPT_TOKENS ||
            item->expectedBytes == NULL || item->expectedLength == 0U ||
            item->expectedLength > UMI_AI_GENERATION_MAX_NEW_TOKENS ||
            score->expectedBytes != item->expectedLength ||
            score->generatedBytes > UMI_AI_GENERATION_MAX_NEW_TOKENS ||
            score->matchingPositionBytes > score->expectedBytes ||
            score->matchingPositionBytes > score->generatedBytes ||
            score->matchingPrefixBytes > score->matchingPositionBytes ||
            (score->exactMatch != 0 && score->exactMatch != 1) ||
            (score->exactMatch != 0 &&
             (score->expectedBytes != score->generatedBytes ||
              score->matchingPositionBytes != score->expectedBytes)))
            return false;
        (void)titleLength;
        /* Bound the total without depending on caller-owned overlarge data. */
        if (expected > SIZE_MAX - score->expectedBytes ||
            generated > SIZE_MAX - score->generatedBytes ||
            positional > SIZE_MAX - score->matchingPositionBytes ||
            prefix > SIZE_MAX - score->matchingPrefixBytes ||
            exact > SIZE_MAX - (size_t)score->exactMatch) return false;
        expected += score->expectedBytes;
        generated += score->generatedBytes;
        positional += score->matchingPositionBytes;
        prefix += score->matchingPrefixBytes;
        exact += (size_t)score->exactMatch;
    }
    return report->expectedBytes == expected &&
           report->generatedBytes == generated &&
           report->matchingPositionBytes == positional &&
           report->matchingPrefixBytes == prefix &&
           report->exactMatchCases == exact;
}

static bool HashIsHex(const char hash[UMI_SHA256_HEX_CAPACITY])
{
    if (hash == NULL || hash[64] != '\0') return false;
    for (size_t i = 0U; i < 64U; ++i)
        if (!((hash[i] >= '0' && hash[i] <= '9') ||
              (hash[i] >= 'a' && hash[i] <= 'f'))) return false;
    return true;
}

UmiStatus UmiAiTrainingBenchmarkEvidenceInspect(
    const UmiAiTrainingDecoder *model,
    const UmiAiTrainingBenchmarkCase *cases,
    const UmiAiTrainingBenchmarkCaseResult *results,
    size_t caseCount,
    const UmiAiTrainingBenchmarkReport *report,
    const UmiAiTrainingSamplingOptions *options,
    uint64_t seed,
    UmiAiTrainingBenchmarkEvidence *outEvidence)
{
    UmiAiTrainingBenchmarkEvidence evidence = {0};
    UmiAiTrainingDecoderConfig config;
    uint64_t steps = 0U;
    UmiSha256 hash;
    UmiStatus status;
    size_t checkpointLength = 0U, written = 0U;
    uint8_t *checkpoint = NULL;
    uint32_t bits;

    if (model == NULL || outEvidence == NULL || options == NULL ||
        !MatchesData(cases, results, caseCount, report) ||
        options->topK == 0U || options->topK >
            UMI_AI_BYTE_VOCABULARY - UMI_AI_BYTE_OFFSET ||
        !isfinite(options->temperature) || options->temperature <= 0.0F)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingDecoderInspect(model, &config, &steps);
    if (status != UMI_STATUS_OK) return status;
    if (config.vocabularySize != UMI_AI_BYTE_VOCABULARY ||
        steps != report->completedOptimiserSteps)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingDecoderCheckpointSize(model, &checkpointLength);
    if (status != UMI_STATUS_OK) return status;
    if (checkpointLength == 0U ||
        checkpointLength > UMI_AI_DECODER_MAX_CHECKPOINT_BYTES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    checkpoint = malloc(checkpointLength);
    if (checkpoint == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = UmiAiTrainingDecoderCheckpointEncode(model, checkpoint,
                                                  checkpointLength, &written);
    if (status == UMI_STATUS_OK && written != checkpointLength)
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK)
        status = UmiSha256Buffer(checkpoint, written,
                                 evidence.modelCheckpointSha256);
    free(checkpoint);
    if (status != UMI_STATUS_OK) return status;

    status = BeginHash(&hash, "UMICOM-AI-REVIEWED-CASE-IDENTITY");
    if (status == UMI_STATUS_OK) status = HashInteger(&hash, (uint64_t)caseCount);
    for (size_t i = 0U; i < caseCount && status == UMI_STATUS_OK; ++i) {
        const UmiAiTrainingBenchmarkCase *item = &cases[i];
        size_t titleLength = 0U;
        (void)CaseTitleLength(item->title, &titleLength);
        status = HashBytes(&hash, item->title, titleLength);
        if (status == UMI_STATUS_OK)
            status = HashBytes(&hash, item->promptBytes, item->promptLength);
        if (status == UMI_STATUS_OK)
            status = HashBytes(&hash, item->expectedBytes, item->expectedLength);
    }
    if (status == UMI_STATUS_OK) status = EndHash(&hash, evidence.benchmarkSuiteSha256);
    if (status != UMI_STATUS_OK) return status;

    memcpy(&bits, &options->temperature, sizeof(bits));
    evidence.samplingSeed = seed;
    evidence.temperatureBits = bits;
    evidence.topK = options->topK;
    status = BeginHash(&hash, "UMICOM-AI-REPORTED-COMPLETION-SCORES");
    if (status == UMI_STATUS_OK) status = HashInteger(&hash, seed);
    if (status == UMI_STATUS_OK) status = HashInteger(&hash, (uint64_t)bits);
    if (status == UMI_STATUS_OK) status = HashInteger(&hash, (uint64_t)options->topK);
    if (status == UMI_STATUS_OK) status = HashInteger(&hash, steps);
    if (status == UMI_STATUS_OK) status = HashInteger(&hash, (uint64_t)caseCount);
    for (size_t i = 0U; i < caseCount && status == UMI_STATUS_OK; ++i) {
        const UmiAiTrainingBenchmarkCaseResult *item = &results[i];
        const uint64_t metrics[] = {
            (uint64_t)item->expectedBytes,
            (uint64_t)item->generatedBytes,
            (uint64_t)item->matchingPositionBytes,
            (uint64_t)item->matchingPrefixBytes,
            (uint64_t)item->exactMatch
        };
        for (size_t j = 0U; j < sizeof(metrics) / sizeof(metrics[0]); ++j) {
            status = HashInteger(&hash, metrics[j]);
            if (status != UMI_STATUS_OK) break;
        }
    }
    if (status == UMI_STATUS_OK) status = EndHash(&hash, evidence.scoredResultsSha256);
    if (status != UMI_STATUS_OK) return status;
    *outEvidence = evidence;
    return UMI_STATUS_OK;
}

typedef struct JsonWriter {
    char *buffer;
    size_t length;
    size_t capacity;
} JsonWriter;

static bool JsonText(JsonWriter *json, const char *text)
{
    const size_t length = strlen(text);
    if (length > json->capacity - json->length) return false;
    memcpy(json->buffer + json->length, text, length);
    json->length += length;
    return true;
}

static bool JsonFormat(JsonWriter *json, const char *format, ...)
{
    int written;
    va_list args;
    if (json->length >= json->capacity) return false;
    va_start(args, format);
    written = vsnprintf(json->buffer + json->length,
                        json->capacity - json->length, format, args);
    va_end(args);
    if (written < 0 || (size_t)written >= json->capacity - json->length)
        return false;
    json->length += (size_t)written;
    return true;
}

static bool JsonTitle(JsonWriter *json, const char *title)
{
    const char *const HEX = "0123456789abcdef";
    if (!JsonText(json, "\"")) return false;
    for (size_t i = 0U; title[i] != '\0'; ++i) {
        const unsigned char value = (unsigned char)title[i];
        if (value == '"' || value == '\\') {
            if (json->length > json->capacity - 2U) return false;
            json->buffer[json->length++] = '\\';
            json->buffer[json->length++] = (char)value;
        } else if (value < 32U) {
            if (json->length > json->capacity - 6U) return false;
            json->buffer[json->length++] = '\\';
            json->buffer[json->length++] = 'u';
            json->buffer[json->length++] = '0';
            json->buffer[json->length++] = '0';
            json->buffer[json->length++] = HEX[value >> 4U];
            json->buffer[json->length++] = HEX[value & 15U];
        } else {
            if (json->length == json->capacity) return false;
            json->buffer[json->length++] = (char)value;
        }
    }
    return JsonText(json, "\"");
}

UmiStatus UmiAiTrainingBenchmarkEvidenceJson(
    const UmiAiTrainingBenchmarkCase *cases,
    const UmiAiTrainingBenchmarkCaseResult *results,
    size_t caseCount,
    const UmiAiTrainingBenchmarkReport *report,
    const UmiAiTrainingBenchmarkEvidence *evidence,
    char *outJson,
    size_t capacity,
    size_t *outWritten)
{
    char staging[UMI_AI_BENCHMARK_EVIDENCE_MAX_JSON_BYTES];
    JsonWriter json = {staging, 0U, sizeof(staging)};
    if (evidence == NULL || outJson == NULL || outWritten == NULL ||
        !MatchesData(cases, results, caseCount, report) ||
        !HashIsHex(evidence->modelCheckpointSha256) ||
        !HashIsHex(evidence->benchmarkSuiteSha256) ||
        !HashIsHex(evidence->scoredResultsSha256) ||
        evidence->topK == 0U || evidence->topK >
             UMI_AI_BYTE_VOCABULARY - UMI_AI_BYTE_OFFSET)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!JsonText(&json, "{\n  \"format\": \"umicom-ai-reviewed-benchmark\",\n") ||
        !JsonFormat(&json,
            "  \"model_checkpoint_sha256\": \"%s\",\n"
            "  \"reviewed_cases_sha256\": \"%s\",\n"
            "  \"reported_scores_sha256\": \"%s\",\n"
            "  \"sampler\": {\"seed\": \"%" PRIu64 "\", "
            "\"temperature_float32_bits\": \"%08" PRIx32 "\", "
            "\"top_k\": %zu},\n",
            evidence->modelCheckpointSha256,
            evidence->benchmarkSuiteSha256,
            evidence->scoredResultsSha256,
            evidence->samplingSeed, evidence->temperatureBits,
            evidence->topK) ||
        !JsonFormat(&json,
            "  \"metrics\": {\"completed_steps\": \"%" PRIu64 "\", "
            "\"case_count\": %zu, \"exact_cases\": %zu, "
            "\"expected_bytes\": %zu, \"generated_bytes\": %zu, "
            "\"matching_position_bytes\": %zu, "
            "\"matching_prefix_bytes\": %zu},\n  \"cases\": [\n",
            report->completedOptimiserSteps, report->caseCount,
            report->exactMatchCases, report->expectedBytes,
            report->generatedBytes, report->matchingPositionBytes,
            report->matchingPrefixBytes))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < caseCount; ++i) {
        const UmiAiTrainingBenchmarkCaseResult *item = &results[i];
        if (!JsonText(&json, "    {\"title\": ") ||
            !JsonTitle(&json, cases[i].title) ||
            !JsonFormat(&json,
                ", \"expected_bytes\": %zu, \"generated_bytes\": %zu, "
                "\"matching_position_bytes\": %zu, "
                "\"matching_prefix_bytes\": %zu, \"exact\": %s}%s\n",
                item->expectedBytes, item->generatedBytes,
                item->matchingPositionBytes, item->matchingPrefixBytes,
                item->exactMatch ? "true" : "false",
                i + 1U == caseCount ? "" : ","))
            return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (!JsonText(&json,
        "  ],\n  \"limitations\": ["
        "\"The benchmark compares bytes; it does not compile or execute C23\", "
        "\"SHA-256 is not a digital signature or proof of origin\", "
        "\"Reported scores require independently trusted execution\"]\n}\n"))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (json.length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    json.buffer[json.length] = '\0';
    memcpy(outJson, json.buffer, json.length + 1U);
    *outWritten = json.length;
    return UMI_STATUS_OK;
}
