/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/experiment_plan.c
 *
 * PURPOSE:
 *   Parse compact, human-reviewed experiment plans deterministically using
 *   C23 alone. Do not introduce an application-local parser or script runner.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/experiment_plan.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define UMI_PLAN_KEY_COUNT 15U

typedef enum PlanKey {
    KEY_MANIFEST = 0,
    KEY_STEPS,
    KEY_EVAL_INTERVAL,
    KEY_EVAL_BATCHES,
    KEY_BATCH_SEQUENCES,
    KEY_LEARNING_RATE,
    KEY_MODEL_SEED,
    KEY_TRAIN_SEED,
    KEY_EVAL_SEED,
    KEY_CONTEXT,
    KEY_CHANNELS,
    KEY_HIDDEN_CHANNELS,
    KEY_ATTENTION_HEADS,
    KEY_EFFECTIVE_SEQUENCES,
    KEY_MICROBATCH_SEQUENCES
} PlanKey;

static const char *const keyNames[UMI_PLAN_KEY_COUNT] = {
    "manifest", "steps", "eval_interval", "eval_batches",
    "batch_sequences", "learning_rate", "model_seed", "train_seed",
    "eval_seed", "context", "channels", "hidden_channels",
    "attention_heads", "effective_sequences", "microbatch_sequences"
};

static bool ParseUnsigned(const char *text, uint64_t *outValue)
{
    uint64_t value = 0U;
    if (text == NULL || outValue == NULL || text[0] == '\0') return false;
    for (const char *cursor = text; *cursor != '\0'; ++cursor) {
        uint64_t digit;
        if (*cursor < '0' || *cursor > '9') return false;
        digit = (uint64_t)(unsigned)(*cursor - '0');
        if (value > (UINT64_MAX - digit) / UINT64_C(10)) return false;
        value = value * UINT64_C(10) + digit;
    }
    *outValue = value;
    return true;
}

/* Decimal learning rates have a bounded, locale-independent representation.
 * Do not accept 'nan', 'inf', exponent syntax or environment-dependent input. */
static bool ParseRate(const char *text, float *outRate)
{
    uint64_t integer = 0U, fraction = 0U, divisor = 1U;
    bool dot = false, any = false, afterDot = false;
    if (text == NULL || outRate == NULL) return false;
    for (const char *cursor = text; *cursor != '\0'; ++cursor) {
        if (*cursor == '.' && !dot) { dot = true; continue; }
        if (*cursor < '0' || *cursor > '9') return false;
        unsigned digit = (unsigned)(*cursor - '0');
        if (dot) {
            if (divisor >= UINT64_C(1000000000)) return false;
            fraction = fraction * UINT64_C(10) + (uint64_t)digit;
            divisor *= UINT64_C(10);
            afterDot = true;
        } else {
            if (integer > UINT64_C(1)) return false;
            integer = integer * UINT64_C(10) + (uint64_t)digit;
        }
        any = true;
    }
    if (!any || (dot && !afterDot)) return false;
    double rate = (double)integer + (double)fraction / (double)divisor;
    if (rate <= 0.0 || rate > 1.0) return false;
    *outRate = (float)rate;
    return true;
}

/* Portable relative POSIX-style components only. A path is an input to a
 * trusted local manifest loader, never an authority or a sandbox boundary. */
static bool IsSafeRelativeManifest(const char *path)
{
    size_t length;
    size_t segmentStart = 0U;
    if (path == NULL) return false;
    length = strlen(path);
    if (length == 0U || length >= UMI_AI_EXPERIMENT_MAX_MANIFEST_RELATIVE)
        return false;
    for (size_t i = 0U; i <= length; ++i) {
        unsigned char ch = (unsigned char)path[i];
        if (ch == '/' || ch == '\0') {
            size_t size = i - segmentStart;
            if (size == 0U || (size == 1U && path[segmentStart] == '.') ||
                (size == 2U && path[segmentStart] == '.' &&
                 path[segmentStart + 1U] == '.')) return false;
            segmentStart = i + 1U;
        } else if (!((ch >= 'A' && ch <= 'Z') ||
                     (ch >= 'a' && ch <= 'z') ||
                     (ch >= '0' && ch <= '9') ||
                     ch == '_' || ch == '-' || ch == '.')) {
            return false;
        }
    }
    return true;
}

static UmiStatus ApplyKey(PlanKey key, const char *value,
                          UmiAiTrainingExperimentConfig *candidate,
                          UmiAiTrainingExperimentAccumulation *accumulation,
                          char *relativeManifest)
{
    uint64_t integer = 0U;
    float rate;
    if (key == KEY_MANIFEST) {
        if (!IsSafeRelativeManifest(value)) return UMI_STATUS_PARSE_ERROR;
        memcpy(relativeManifest, value, strlen(value) + 1U);
        return UMI_STATUS_OK;
    }
    if (key == KEY_LEARNING_RATE) {
        if (!ParseRate(value, &rate)) return UMI_STATUS_PARSE_ERROR;
        candidate->learningRate = rate;
        return UMI_STATUS_OK;
    }
    if ((key == KEY_EFFECTIVE_SEQUENCES ||
         key == KEY_MICROBATCH_SEQUENCES) && accumulation == NULL)
        return UMI_STATUS_PARSE_ERROR;
    if (!ParseUnsigned(value, &integer)) return UMI_STATUS_PARSE_ERROR;
    if (key != KEY_MODEL_SEED && key != KEY_TRAIN_SEED &&
        key != KEY_EVAL_SEED && integer > UINT32_MAX)
        return UMI_STATUS_PARSE_ERROR;
    switch (key) {
    case KEY_STEPS: candidate->steps = (uint32_t)integer; break;
    case KEY_EVAL_INTERVAL: candidate->evalInterval = (uint32_t)integer; break;
    case KEY_EVAL_BATCHES: candidate->evalBatches = (uint32_t)integer; break;
    case KEY_BATCH_SEQUENCES: candidate->batchSequences = (uint32_t)integer; break;
    case KEY_MODEL_SEED: candidate->decoder.initialSeed = integer; break;
    case KEY_TRAIN_SEED: candidate->trainSeed = integer; break;
    case KEY_EVAL_SEED: candidate->evalSeed = integer; break;
    case KEY_CONTEXT: candidate->decoder.contextLength = (uint32_t)integer; break;
    case KEY_CHANNELS: candidate->decoder.channels = (uint32_t)integer; break;
    case KEY_HIDDEN_CHANNELS: candidate->decoder.hiddenChannels = (uint32_t)integer; break;
    case KEY_ATTENTION_HEADS: candidate->decoder.attentionHeads = (uint32_t)integer; break;
    case KEY_EFFECTIVE_SEQUENCES:
        accumulation->effectiveSequences = (uint32_t)integer; break;
    case KEY_MICROBATCH_SEQUENCES:
        accumulation->microbatchSequences = (uint32_t)integer; break;
    default: return UMI_STATUS_PARSE_ERROR;
    }
    return UMI_STATUS_OK;
}

static UmiStatus ParsePlanInternal(
    const char *text, size_t textLength,
    UmiAiTrainingExperimentConfig *outConfig,
    UmiAiTrainingExperimentAccumulation *outAccumulation,
    char *outRelativeManifest, size_t manifestCapacity)
{
    UmiAiTrainingExperimentConfig candidate;
    UmiAiTrainingExperimentAccumulation selectedAccumulation = {0};
    char relativeManifest[UMI_AI_EXPERIMENT_MAX_MANIFEST_RELATIVE] = {0};
    uint32_t seen = 0U;
    size_t cursor = 0U;
    bool headerFound = false;
    UmiStatus status;
    if (text == NULL || outConfig == NULL || outRelativeManifest == NULL ||
        textLength == 0U || textLength > UMI_AI_EXPERIMENT_MAX_PLAN_BYTES)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingExperimentDefaults(&candidate);
    if (status != UMI_STATUS_OK) return status;
    while (cursor < textLength) {
        char line[UMI_AI_EXPERIMENT_MAX_PLAN_BYTES + 1U];
        size_t start = cursor;
        size_t length;
        char *separator;
        PlanKey key = UMI_PLAN_KEY_COUNT;
        while (cursor < textLength && text[cursor] != '\n') {
            if (text[cursor] == '\0') return UMI_STATUS_PARSE_ERROR;
            ++cursor;
        }
        length = cursor - start;
        if (cursor < textLength) ++cursor;
        if (length > 0U && text[start + length - 1U] == '\r') --length;
        if (length == 0U) continue;
        memcpy(line, text + start, length);
        line[length] = '\0';
        if (line[0] == '#') continue;
        if (!headerFound) {
            if (strcmp(line, "UMICOM-AI-EXPERIMENT-1") != 0)
                return UMI_STATUS_PARSE_ERROR;
            headerFound = true;
            continue;
        }
        separator = strchr(line, '=');
        if (separator == NULL || separator == line || separator[1] == '\0')
            return UMI_STATUS_PARSE_ERROR;
        *separator = '\0';
        for (size_t i = 0U; i < UMI_PLAN_KEY_COUNT; ++i) {
            if (strcmp(keyNames[i], line) == 0) {
                key = (PlanKey)i;
                break;
            }
        }
        if (key == UMI_PLAN_KEY_COUNT ||
            (seen & (UINT32_C(1) << (unsigned)key)) != 0U)
            return UMI_STATUS_PARSE_ERROR;
        status = ApplyKey(key, separator + 1U, &candidate,
                          outAccumulation == NULL ? NULL : &selectedAccumulation,
                          relativeManifest);
        if (status != UMI_STATUS_OK) return status;
        seen |= UINT32_C(1) << (unsigned)key;
    }
    if (!headerFound || (seen & UINT32_C(1)) == 0U)
        return UMI_STATUS_PARSE_ERROR;
    status = UmiAiTrainingExperimentValidate(&candidate);
    if (status != UMI_STATUS_OK) return status;
    if (outAccumulation != NULL) {
        const uint32_t chosen = seen &
            ((UINT32_C(1) << KEY_EFFECTIVE_SEQUENCES) |
             (UINT32_C(1) << KEY_MICROBATCH_SEQUENCES));
        const uint32_t both = (UINT32_C(1) << KEY_EFFECTIVE_SEQUENCES) |
                              (UINT32_C(1) << KEY_MICROBATCH_SEQUENCES);
        /* A single key is ambiguous: never silently use a fallback batch
         * size while the author believes both values were reviewed. */
        if (chosen != 0U && chosen != both) return UMI_STATUS_PARSE_ERROR;
        if (chosen == both) {
            status = UmiAiTrainingExperimentAccumulationValidate(
                &candidate, &selectedAccumulation);
            if (status != UMI_STATUS_OK) return status;
        }
    }
    size_t manifestLength = strlen(relativeManifest) + 1U;
    if (manifestLength > manifestCapacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    *outConfig = candidate;
    if (outAccumulation != NULL) *outAccumulation = selectedAccumulation;
    memcpy(outRelativeManifest, relativeManifest, manifestLength);
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingExperimentPlanParse(
    const char *text, size_t textLength,
    UmiAiTrainingExperimentConfig *outConfig,
    char *outRelativeManifest, size_t manifestCapacity)
{
    return ParsePlanInternal(text, textLength, outConfig, NULL,
                             outRelativeManifest, manifestCapacity);
}

UmiStatus UmiAiTrainingExperimentPlanParseAccumulated(
    const char *text, size_t textLength,
    UmiAiTrainingExperimentConfig *outConfig,
    UmiAiTrainingExperimentAccumulation *outAccumulation,
    char *outRelativeManifest, size_t manifestCapacity)
{
    if (outAccumulation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return ParsePlanInternal(text, textLength, outConfig, outAccumulation,
                             outRelativeManifest, manifestCapacity);
}

static UmiStatus LoadPlanInternal(
    const char *planFile,
    UmiAiTrainingExperimentConfig *outConfig,
    UmiAiTrainingExperimentAccumulation *outAccumulation,
    char *outManifestPath, size_t manifestCapacity)
{
    char text[UMI_AI_EXPERIMENT_MAX_PLAN_BYTES + 1U];
    char relative[UMI_AI_EXPERIMENT_MAX_MANIFEST_RELATIVE];
    UmiAiTrainingExperimentConfig candidate;
    UmiAiTrainingExperimentAccumulation selectedAccumulation = {0};
    const char *separator = NULL;
    FILE *file;
    long length;
    size_t prefixLength, relativeLength;
    UmiStatus status;
    if (planFile == NULL || planFile[0] == '\0' || outConfig == NULL ||
        outManifestPath == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    file = fopen(planFile, "rb");
    if (file == NULL) return UMI_STATUS_IO_ERROR;
    if (fseek(file, 0L, SEEK_END) != 0 || (length = ftell(file)) < 0L) {
        fclose(file);
        return UMI_STATUS_IO_ERROR;
    }
    if ((unsigned long)length > UMI_AI_EXPERIMENT_MAX_PLAN_BYTES) {
        fclose(file);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (fseek(file, 0L, SEEK_SET) != 0) {
        fclose(file);
        return UMI_STATUS_IO_ERROR;
    }
    if (fread(text, 1U, (size_t)length, file) != (size_t)length ||
        fgetc(file) != EOF || ferror(file) != 0) {
        fclose(file);
        return UMI_STATUS_IO_ERROR;
    }
    if (fclose(file) != 0) return UMI_STATUS_IO_ERROR;
    status = ParsePlanInternal(text, (size_t)length, &candidate,
                              outAccumulation == NULL ? NULL : &selectedAccumulation,
                              relative, sizeof(relative));
    if (status != UMI_STATUS_OK) return status;
    /* Both Windows and POSIX plan paths use their final separator. Relative
     * manifest fragments always use /, independent of cwd. */
    for (const char *cursor = planFile; *cursor != '\0'; ++cursor) {
        if (*cursor == '/' || *cursor == '\\') separator = cursor;
    }
    prefixLength = separator == NULL ? 0U :
                   (size_t)(separator - planFile) + 1U;
    relativeLength = strlen(relative);
    if (prefixLength > SIZE_MAX - relativeLength - 1U ||
        prefixLength + relativeLength + 1U > manifestCapacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(outManifestPath, planFile, prefixLength);
    memcpy(outManifestPath + prefixLength, relative, relativeLength + 1U);
    *outConfig = candidate;
    if (outAccumulation != NULL) *outAccumulation = selectedAccumulation;
    return UMI_STATUS_OK;
}

/* Both file loaders retain one strict byte reader and CWD-independent path
 * resolver. Only the opt-in loader accepts effective/microbatch keys. */
UmiStatus UmiAiTrainingExperimentPlanLoadFile(
    const char *planFile,
    UmiAiTrainingExperimentConfig *outConfig,
    char *outManifestPath, size_t manifestCapacity)
{
    return LoadPlanInternal(planFile, outConfig, NULL,
                            outManifestPath, manifestCapacity);
}

UmiStatus UmiAiTrainingExperimentPlanLoadAccumulated(
    const char *planFile,
    UmiAiTrainingExperimentConfig *outConfig,
    UmiAiTrainingExperimentAccumulation *outAccumulation,
    char *outManifestPath, size_t manifestCapacity)
{
    if (outAccumulation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return LoadPlanInternal(planFile, outConfig, outAccumulation,
                            outManifestPath, manifestCapacity);
}
