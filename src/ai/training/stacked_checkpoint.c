/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/stacked_checkpoint.c
 * PURPOSE: Lossless portable FP32 checkpoints for the native stacked decoder;
 *          reuse the canonical Framework SHA-256, not a new hash function.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/stacked_checkpoint.h"
#include "stacked_decoder_internal.h"
#include <float.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define STACKED_HEADER_BYTES 60U
#define STACKED_TRAILER_BYTES 32U
static const uint8_t STACKED_MAGIC[8] = {'U','M','I','S','T','K','0','1'};

static void Write32(uint8_t *out, uint32_t value)
{
    for (size_t i = 0U; i < 4U; ++i)
        out[i] = (uint8_t)(value >> (8U * i));
}
static void Write64(uint8_t *out, uint64_t value)
{
    for (size_t i = 0U; i < 8U; ++i)
        out[i] = (uint8_t)(value >> (8U * i));
}
static uint32_t Read32(const uint8_t *in)
{
    uint32_t value = 0U;
    for (size_t i = 0U; i < 4U; ++i)
        value |= (uint32_t)in[i] << (8U * i);
    return value;
}
static uint64_t Read64(const uint8_t *in)
{
    uint64_t value = 0U;
    for (size_t i = 0U; i < 8U; ++i)
        value |= (uint64_t)in[i] << (8U * i);
    return value;
}
static int PortableFloat(void)
{
    return sizeof(float) == 4U && FLT_RADIX == 2 &&
        FLT_MANT_DIG == 24 && FLT_MAX_EXP == 128;
}
static UmiStatus DigestBytes(const uint8_t *bytes, size_t length,
                             uint8_t outDigest[UMI_SHA256_BYTES])
{
    UmiSha256 state;
    UmiStatus status;
    UmiSha256Init(&state);
    status = UmiSha256Update(&state, bytes, length);
    if (status == UMI_STATUS_OK)
        status = UmiSha256Final(&state, outDigest);
    return status;
}

static UmiStatus ExpectedSize(
    const UmiAiTrainingStackedDecoderConfig *config,
    size_t *outCount, size_t *outValues, size_t *outBytes)
{
    size_t count, total = 0U;
    if (config == NULL || outCount == NULL ||
        outValues == NULL || outBytes == NULL ||
        UmiAiStackedValidateConfig(config) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    count = 3U + (size_t)config->transformerLayers *
            UMI_AI_STACKED_PARAMETERS_PER_LAYER;
    for (size_t index = 0U; index < count; ++index) {
        size_t rows, columns;
        UmiAiStackedParameterShape(config, index, &rows, &columns);
        if (rows == 0U || columns == 0U ||
            rows > SIZE_MAX / columns ||
            total > SIZE_MAX - rows * columns)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        total += rows * columns;
    }
    if (count > UINT32_MAX || total > UINT32_MAX ||
        count > (SIZE_MAX - STACKED_HEADER_BYTES -
                 STACKED_TRAILER_BYTES) / 8U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    const size_t fixed = STACKED_HEADER_BYTES + count * 8U +
                         STACKED_TRAILER_BYTES;
    if (total > (SIZE_MAX - fixed) / 4U ||
        fixed + total * 4U > UMI_AI_STACKED_CHECKPOINT_MAX_BYTES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    *outCount = count;
    *outValues = total;
    *outBytes = fixed + total * 4U;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingStackedCheckpointSize(
    const UmiAiTrainingStackedDecoder *model, size_t *outBytes)
{
    size_t count = 0U, values = 0U, bytes = 0U;
    UmiStatus status;
    if (model == NULL || outBytes == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!PortableFloat()) return UMI_STATUS_UNAVAILABLE;
    status = ExpectedSize(&model->config, &count, &values, &bytes);
    if (status != UMI_STATUS_OK) return status;
    if (count != model->parameterCount || values != model->parameterElements)
        return UMI_STATUS_INVALID_STATE;
    *outBytes = bytes;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingStackedCheckpointExport(
    const UmiAiTrainingStackedDecoder *model,
    uint8_t *outBytes, size_t capacity, size_t *outWritten)
{
    uint8_t *staged = NULL;
    UmiStatus status;
    size_t count = 0U, elements = 0U, required = 0U;
    size_t cursor;
    if (model == NULL || outBytes == NULL || outWritten == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingStackedCheckpointSize(model, &required);
    if (status != UMI_STATUS_OK) return status;
    if (capacity < required) return UMI_STATUS_CAPACITY_EXCEEDED;
    status = ExpectedSize(&model->config, &count, &elements, &required);
    if (status != UMI_STATUS_OK) return status;
    staged = malloc(required);
    if (staged == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(staged, STACKED_MAGIC, sizeof(STACKED_MAGIC));
    Write32(staged + 8U, UMI_AI_STACKED_CHECKPOINT_FORMAT);
    Write32(staged + 12U, model->config.vocabularySize);
    Write32(staged + 16U, model->config.contextLength);
    Write32(staged + 20U, model->config.channels);
    Write32(staged + 24U, model->config.hiddenChannels);
    Write32(staged + 28U, model->config.attentionHeads);
    Write32(staged + 32U, model->config.transformerLayers);
    Write64(staged + 36U, model->config.initialSeed);
    Write64(staged + 44U, model->completedSteps);
    Write32(staged + 52U, (uint32_t)count);
    Write32(staged + 56U, (uint32_t)elements);
    cursor = STACKED_HEADER_BYTES;
    for (size_t index = 0U; index < count; ++index) {
        size_t rows, columns;
        UmiAiStackedParameterShape(&model->config, index, &rows, &columns);
        Write32(staged + cursor, (uint32_t)rows);
        Write32(staged + cursor + 4U, (uint32_t)columns);
        cursor += 8U;
    }
    for (size_t index = 0U; index < count; ++index) {
        size_t rows, columns, valuesCount;
        float *values;
        UmiAiStackedParameterShape(&model->config, index, &rows, &columns);
        valuesCount = rows * columns;
        values = malloc(valuesCount * sizeof(*values));
        if (values == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto done; }
        status = UmiAiTrainingTensorValues(model->parameters[index],
                                            values, valuesCount);
        if (status == UMI_STATUS_OK) {
            for (size_t j = 0U; j < valuesCount; ++j) {
                uint32_t bits = 0U;
                if (!isfinite(values[j])) {
                    status = UMI_STATUS_INVALID_STATE;
                    break;
                }
                memcpy(&bits, &values[j], sizeof(bits));
                Write32(staged + cursor, bits);
                cursor += 4U;
            }
        }
        free(values);
        if (status != UMI_STATUS_OK) goto done;
    }
    if (cursor != required - STACKED_TRAILER_BYTES) {
        status = UMI_STATUS_INTERNAL_ERROR;
        goto done;
    }
    status = DigestBytes(staged, cursor, staged + cursor);
    if (status == UMI_STATUS_OK) {
        memcpy(outBytes, staged, required);
        *outWritten = required;
    }
done:
    free(staged);
    return status;
}

UmiStatus UmiAiTrainingStackedCheckpointInspect(
    const uint8_t *bytes, size_t byteCount,
    UmiAiTrainingStackedCheckpointInfo *outInfo)
{
    UmiAiTrainingStackedCheckpointInfo candidate = {0};
    uint8_t digest[UMI_SHA256_BYTES] = {0};
    size_t parameterCount = 0U, parameterElements = 0U, expected = 0U;
    UmiStatus status;
    size_t cursor;
    if (bytes == NULL || outInfo == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!PortableFloat()) return UMI_STATUS_UNAVAILABLE;
    if (byteCount < STACKED_HEADER_BYTES + STACKED_TRAILER_BYTES ||
        byteCount > UMI_AI_STACKED_CHECKPOINT_MAX_BYTES ||
        memcmp(bytes, STACKED_MAGIC, sizeof(STACKED_MAGIC)) != 0 ||
        Read32(bytes + 8U) != UMI_AI_STACKED_CHECKPOINT_FORMAT)
        return UMI_STATUS_PARSE_ERROR;
    candidate.config = (UmiAiTrainingStackedDecoderConfig){
        Read32(bytes + 12U), Read32(bytes + 16U), Read32(bytes + 20U),
        Read32(bytes + 24U), Read32(bytes + 28U), Read32(bytes + 32U),
        Read64(bytes + 36U)
    };
    candidate.completedTrainingSteps = Read64(bytes + 44U);
    status = ExpectedSize(&candidate.config, &parameterCount,
                          &parameterElements, &expected);
    if (status != UMI_STATUS_OK || expected != byteCount ||
        Read32(bytes + 52U) != (uint32_t)parameterCount ||
        Read32(bytes + 56U) != (uint32_t)parameterElements)
        return UMI_STATUS_PARSE_ERROR;
    status = DigestBytes(bytes, byteCount - STACKED_TRAILER_BYTES, digest);
    if (status != UMI_STATUS_OK) return status;
    if (memcmp(digest, bytes + byteCount - STACKED_TRAILER_BYTES,
               UMI_SHA256_BYTES) != 0)
        return UMI_STATUS_PARSE_ERROR;
    cursor = STACKED_HEADER_BYTES;
    for (size_t index = 0U; index < parameterCount; ++index) {
        size_t rows, columns;
        UmiAiStackedParameterShape(&candidate.config, index, &rows, &columns);
        if (Read32(bytes + cursor) != (uint32_t)rows ||
            Read32(bytes + cursor + 4U) != (uint32_t)columns)
            return UMI_STATUS_PARSE_ERROR;
        cursor += 8U;
    }
    for (size_t index = 0U; index < parameterElements; ++index) {
        uint32_t bits = Read32(bytes + cursor);
        float value;
        memcpy(&value, &bits, sizeof(value));
        if (!isfinite(value)) return UMI_STATUS_PARSE_ERROR;
        cursor += 4U;
    }
    if (cursor != byteCount - STACKED_TRAILER_BYTES)
        return UMI_STATUS_PARSE_ERROR;
    candidate.parameterTensors = parameterCount;
    candidate.parameterElements = parameterElements;
    candidate.checkpointBytes = byteCount;
    memcpy(candidate.sha256, digest, sizeof(digest));
    *outInfo = candidate;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingStackedCheckpointDecode(
    const uint8_t *bytes, size_t byteCount,
    UmiAiTrainingStackedDecoder **outModel)
{
    UmiAiTrainingStackedCheckpointInfo info = {0};
    float *values = NULL;
    const float *parameters[UMI_AI_STACKED_MAX_PARAMETERS] = {0};
    size_t cursor, totalBytes;
    UmiStatus status;
    if (outModel == NULL || *outModel != NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingStackedCheckpointInspect(bytes, byteCount, &info);
    if (status != UMI_STATUS_OK) return status;
    if (info.parameterElements > SIZE_MAX / sizeof(float))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    totalBytes = info.parameterElements * sizeof(float);
    values = malloc(totalBytes);
    if (values == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    cursor = STACKED_HEADER_BYTES + info.parameterTensors * 8U;
    size_t offset = 0U;
    for (size_t index = 0U; index < info.parameterTensors; ++index) {
        size_t rows, columns;
        UmiAiStackedParameterShape(&info.config, index, &rows, &columns);
        parameters[index] = values + offset;
        for (size_t j = 0U; j < rows * columns; ++j) {
            uint32_t bits = Read32(bytes + cursor);
            memcpy(&values[offset + j], &bits, sizeof(bits));
            cursor += 4U;
        }
        offset += rows * columns;
    }
    status = UmiAiStackedCreateFromValues(&info.config, parameters,
                 info.completedTrainingSteps, outModel);
    free(values);
    return status;
}
