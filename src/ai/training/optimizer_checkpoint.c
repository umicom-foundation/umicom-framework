/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/optimizer_checkpoint.c
 *
 * PURPOSE:
 *   Preserve AdamW's bias-correction step and exact FP64 moment history in
 *   the same portable checkpoint envelope as the canonical FP32 decoder.
 *
 * SAFETY AND OWNERSHIP:
 *   Reject malformed/untrusted bytes before constructing decoder state.
 *   Both output handles are published together only after complete validation.
 *   Native file publication is exclusive and never overwrites checkpoints.
 *   The FNV checksum is for accidental corruption, NOT authentication.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation 
 * LICENCE: MIT
 */
#include "umicom/ai/training/optimizer_checkpoint.h"
#include "umicom/ai/training/checkpoint.h"
#include "optimizer_internal.h"

#include <errno.h>
#include <stdbool.h>
#include <float.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#include <fcntl.h>
#include <io.h>
#include <sys/stat.h>
#include <windows.h>
#else
#include <fcntl.h>
#include <unistd.h>
#endif

#if FLT_RADIX != 2 || FLT_MANT_DIG != 24 || FLT_MAX_EXP != 128 || \
    DBL_MANT_DIG != 53 || DBL_MAX_EXP != 1024
#error Portable AdamW checkpoints require IEEE754 binary32 and binary64.
#endif
_Static_assert(sizeof(float) == 4U && sizeof(double) == 8U,
               "AdamW checkpoint floating-point size mismatch");

#define UMI_ADAMW_MAGIC "UMIADW01"
#define UMI_ADAMW_FORMAT UINT32_C(1)
#define UMI_ADAMW_HEADER 48U
#define UMI_ADAMW_TRAILER 8U

static void Put32(uint8_t *bytes, size_t *position, uint32_t value)
{
    for (size_t i = 0U; i < 4U; ++i)
        bytes[(*position)++] = (uint8_t)(value >> (8U * i));
}

static void Put64(uint8_t *bytes, size_t *position, uint64_t value)
{
    for (size_t i = 0U; i < 8U; ++i)
        bytes[(*position)++] = (uint8_t)(value >> (8U * i));
}

static uint32_t Get32(const uint8_t *bytes, size_t *position)
{
    uint32_t value = 0U;
    for (size_t i = 0U; i < 4U; ++i)
        value |= (uint32_t)bytes[(*position)++] << (8U * i);
    return value;
}

static uint64_t Get64(const uint8_t *bytes, size_t *position)
{
    uint64_t value = 0U;
    for (size_t i = 0U; i < 8U; ++i)
        value |= (uint64_t)bytes[(*position)++] << (8U * i);
    return value;
}

static uint64_t Checksum(const uint8_t *data, size_t count)
{
    uint64_t value = UINT64_C(14695981039346656037);
    for (size_t i = 0U; i < count; ++i) {
        value ^= (uint64_t)data[i];
        value *= UINT64_C(1099511628211);
    }
    return value;
}

static bool ValidAdamW(const UmiAiTrainingAdamWConfig *config)
{
    return config != NULL &&
        isfinite(config->beta1) && config->beta1 >= 0.0F && config->beta1 < 1.0F &&
        isfinite(config->beta2) && config->beta2 >= 0.0F && config->beta2 < 1.0F &&
        isfinite(config->epsilon) && config->epsilon > 0.0F &&
        isfinite(config->weightDecay) && config->weightDecay >= 0.0F;
}

/* Overflow-safe envelope sizing. Never trust on-disk dimensions. */
static UmiStatus RequiredSize(size_t decoderBytes,
                              size_t parameters, size_t elements,
                              size_t *outSize)
{
    const size_t fixed = UMI_ADAMW_HEADER + UMI_ADAMW_TRAILER;
    size_t maxBytes = UMI_AI_ADAMW_MAX_CHECKPOINT_BYTES;
    if (outSize == NULL || decoderBytes == 0U ||
        decoderBytes > UMI_AI_DECODER_MAX_CHECKPOINT_BYTES ||
        parameters == 0U || parameters > UMI_AI_TRAINING_MAX_TENSORS ||
        elements == 0U || elements > UINT32_MAX ||
        fixed > maxBytes || parameters > (maxBytes - fixed) / 4U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t required = fixed + parameters * 4U;
    if (decoderBytes > maxBytes - required) return UMI_STATUS_CAPACITY_EXCEEDED;
    required += decoderBytes;
    if (elements > (maxBytes - required) / (2U * sizeof(double)))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    required += elements * (2U * sizeof(double));
    *outSize = required;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingAdamWCheckpointSize(
    const UmiAiTrainingDecoder *model,
    const UmiAiTrainingAdamW *optimizer, size_t *outSize)
{
    UmiAiTrainingAdamWConfig config;
    const size_t *counts = NULL;
    const double *first = NULL, *second = NULL;
    uint64_t steps = 0U;
    size_t parameters = 0U, elements = 0U, decoderBytes = 0U;
    UmiStatus status;
    if (model == NULL || optimizer == NULL || outSize == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiDecoderAdamWCheckBinding(model, optimizer);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingAdamWInternalInspect(optimizer, &config, &steps,
        &parameters, &elements, &counts, &first, &second);
    if (status != UMI_STATUS_OK) return status;
    if (!ValidAdamW(&config)) return UMI_STATUS_INVALID_STATE;
    status = UmiAiTrainingDecoderCheckpointSize(model, &decoderBytes);
    if (status != UMI_STATUS_OK) return status;
    return RequiredSize(decoderBytes, parameters, elements, outSize);
}

UmiStatus UmiAiTrainingAdamWCheckpointEncode(
    const UmiAiTrainingDecoder *model,
    const UmiAiTrainingAdamW *optimizer,
    uint8_t *outBytes, size_t capacity, size_t *outWritten)
{
    UmiAiTrainingAdamWConfig config;
    const size_t *counts = NULL;
    const double *first = NULL, *second = NULL;
    uint64_t steps;
    size_t parameters, elements, decoderBytes, required = 0U, position;
    size_t written = 0U;
    uint8_t *temporary = NULL;
    UmiStatus status;
    if (model == NULL || optimizer == NULL || outBytes == NULL || outWritten == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingAdamWCheckpointSize(model, optimizer, &required);
    if (status != UMI_STATUS_OK) return status;
    if (capacity < required) return UMI_STATUS_CAPACITY_EXCEEDED;
    status = UmiAiTrainingAdamWInternalInspect(optimizer, &config, &steps,
        &parameters, &elements, &counts, &first, &second);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingDecoderCheckpointSize(model, &decoderBytes);
    if (status != UMI_STATUS_OK) return status;
    /* Every moment is checked before the first caller-owned byte is touched. */
    for (size_t i = 0U; i < elements; ++i) {
        if (!isfinite(first[i]) || !isfinite(second[i]) || second[i] < 0.0)
            return UMI_STATUS_INVALID_STATE;
    }
    temporary = malloc(required);
    if (temporary == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(temporary, UMI_ADAMW_MAGIC, 8U);
    position = 8U;
    Put32(temporary, &position, UMI_ADAMW_FORMAT);
    Put32(temporary, &position, (uint32_t)decoderBytes);
    Put32(temporary, &position, (uint32_t)parameters);
    Put32(temporary, &position, (uint32_t)elements);
    Put64(temporary, &position, steps);
    const float rates[4] = {config.beta1, config.beta2,
                            config.epsilon, config.weightDecay};
    for (size_t i = 0U; i < 4U; ++i) {
        uint32_t bits;
        memcpy(&bits, &rates[i], sizeof(bits));
        Put32(temporary, &position, bits);
    }
    for (size_t i = 0U; i < parameters; ++i) {
        if (counts[i] > UINT32_MAX) { status = UMI_STATUS_CAPACITY_EXCEEDED; goto fail; }
        Put32(temporary, &position, (uint32_t)counts[i]);
    }
    status = UmiAiTrainingDecoderCheckpointEncode(model, temporary + position,
                                                 decoderBytes, &written);
    if (status != UMI_STATUS_OK) goto fail;
    if (written != decoderBytes) { status = UMI_STATUS_INTERNAL_ERROR; goto fail; }
    position += decoderBytes;
    for (size_t i = 0U; i < elements; ++i) {
        uint64_t bits;
        memcpy(&bits, &first[i], sizeof(bits));
        Put64(temporary, &position, bits);
        memcpy(&bits, &second[i], sizeof(bits));
        Put64(temporary, &position, bits);
    }
    if (position + UMI_ADAMW_TRAILER != required) {
        status = UMI_STATUS_INTERNAL_ERROR;
        goto fail;
    }
    Put64(temporary, &position, Checksum(temporary, required - UMI_ADAMW_TRAILER));
    memcpy(outBytes, temporary, required);
    *outWritten = required;
    free(temporary);
    return UMI_STATUS_OK;
fail:
    free(temporary);
    return status;
}

UmiStatus UmiAiTrainingAdamWCheckpointDecode(
    const uint8_t *bytes, size_t byteCount,
    UmiAiTrainingDecoder **outModel,
    UmiAiTrainingAdamW **outOptimizer)
{
    UmiAiTrainingAdamWConfig config = {0};
    UmiAiTrainingAdamWConfig actual;
    UmiAiTrainingDecoder *decodedModel = NULL;
    UmiAiTrainingAdamW *decodedOptimizer = NULL;
    const size_t *actualCounts = NULL;
    const double *actualFirst = NULL, *actualSecond = NULL;
    size_t position = 8U, required = 0U;
    size_t decoderBytes, parameters, elements, actualParameters, actualElements;
    uint64_t steps, actualSteps, modelSteps;
    uint32_t counts[UMI_AI_TRAINING_MAX_TENSORS] = {0};
    double *first = NULL, *second = NULL;
    UmiAiTrainingDecoderConfig modelConfig;
    UmiStatus status = UMI_STATUS_PARSE_ERROR;
    if (bytes == NULL || outModel == NULL || outOptimizer == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (byteCount < UMI_ADAMW_HEADER + UMI_ADAMW_TRAILER ||
        byteCount > UMI_AI_ADAMW_MAX_CHECKPOINT_BYTES ||
        memcmp(bytes, UMI_ADAMW_MAGIC, 8U) != 0)
        return UMI_STATUS_PARSE_ERROR;
    if (Get32(bytes, &position) != UMI_ADAMW_FORMAT) return UMI_STATUS_PARSE_ERROR;
    decoderBytes = (size_t)Get32(bytes, &position);
    parameters = (size_t)Get32(bytes, &position);
    elements = (size_t)Get32(bytes, &position);
    steps = Get64(bytes, &position);
    float *options[4] = {&config.beta1, &config.beta2,
                         &config.epsilon, &config.weightDecay};
    for (size_t i = 0U; i < 4U; ++i) {
        uint32_t bits = Get32(bytes, &position);
        memcpy(options[i], &bits, sizeof(bits));
    }
    if (!ValidAdamW(&config) ||
        RequiredSize(decoderBytes, parameters, elements, &required) != UMI_STATUS_OK ||
        byteCount != required) return UMI_STATUS_PARSE_ERROR;
    size_t checksumPos = byteCount - UMI_ADAMW_TRAILER;
    if (Checksum(bytes, checksumPos) != Get64(bytes, &checksumPos))
        return UMI_STATUS_PARSE_ERROR;
    size_t sum = 0U;
    for (size_t i = 0U; i < parameters; ++i) {
        counts[i] = Get32(bytes, &position);
        if (counts[i] == 0U || counts[i] > elements - sum)
            return UMI_STATUS_PARSE_ERROR;
        sum += (size_t)counts[i];
    }
    if (sum != elements) return UMI_STATUS_PARSE_ERROR;
    /* Decode the nested canonical model, including its independent checksum. */
    status = UmiAiTrainingDecoderCheckpointDecode(bytes + position,
                                                  decoderBytes, &decodedModel);
    if (status != UMI_STATUS_OK) goto fail;
    position += decoderBytes;
    status = UmiAiTrainingDecoderInspect(decodedModel, &modelConfig, &modelSteps);
    if (status != UMI_STATUS_OK) goto fail;
    if (modelSteps != steps) { status = UMI_STATUS_PARSE_ERROR; goto fail; }
    status = UmiAiDecoderAdamWRestoreCreate(decodedModel, &config,
                                           &decodedOptimizer);
    if (status != UMI_STATUS_OK) goto fail;
    status = UmiAiTrainingAdamWInternalInspect(decodedOptimizer,
        &actual, &actualSteps, &actualParameters, &actualElements,
        &actualCounts, &actualFirst, &actualSecond);
    if (status != UMI_STATUS_OK) goto fail;
    if (actualParameters != parameters || actualElements != elements) {
        status = UMI_STATUS_PARSE_ERROR; goto fail;
    }
    for (size_t i = 0U; i < parameters; ++i) {
        if (actualCounts[i] != (size_t)counts[i]) {
            status = UMI_STATUS_PARSE_ERROR; goto fail;
        }
    }
    if (elements > SIZE_MAX / sizeof(double)) {
        status = UMI_STATUS_CAPACITY_EXCEEDED; goto fail;
    }
    first = malloc(elements * sizeof(*first));
    second = malloc(elements * sizeof(*second));
    if (first == NULL || second == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto fail; }
    for (size_t i = 0U; i < elements; ++i) {
        uint64_t bits = Get64(bytes, &position);
        memcpy(&first[i], &bits, sizeof(bits));
        bits = Get64(bytes, &position);
        memcpy(&second[i], &bits, sizeof(bits));
    }
    status = UmiAiTrainingAdamWInternalRestore(decodedOptimizer,
                                               first, second, elements, steps);
    if (status != UMI_STATUS_OK) goto fail;
    status = UmiAiDecoderAdamWCheckBinding(decodedModel, decodedOptimizer);
    if (status != UMI_STATUS_OK) goto fail;
    *outModel = decodedModel;
    *outOptimizer = decodedOptimizer;
    free(first); free(second);
    return UMI_STATUS_OK;
fail:
    free(first); free(second);
    UmiAiTrainingAdamWDestroy(decodedOptimizer);
    UmiAiTrainingDecoderDestroy(decodedModel);
    return status;
}

/* This code mirrors the approved decoder checkpoint publication semantics:
 * same-directory exclusive staging, explicit write/sync, no overwrite. We
 * preserve the original decoder persistence implementation unchanged. */
UmiStatus UmiAiTrainingAdamWCheckpointSaveNewFile(
    const UmiAiTrainingDecoder *model,
    const UmiAiTrainingAdamW *optimizer, const char *path)
{
    size_t required = 0U, written = 0U;
    uint8_t *bytes = NULL;
    char *stage = NULL;
    int descriptor = -1;
    UmiStatus status;
    size_t pathLength;
    if (model == NULL || optimizer == NULL || path == NULL || path[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    pathLength = strlen(path);
    if (pathLength > 4000U) return UMI_STATUS_CAPACITY_EXCEEDED;
    status = UmiAiTrainingAdamWCheckpointSize(model, optimizer, &required);
    if (status != UMI_STATUS_OK) return status;
    bytes = malloc(required);
    stage = malloc(pathLength + sizeof(".umi-adamw-partial"));
    if (bytes == NULL || stage == NULL) {
        status = UMI_STATUS_OUT_OF_MEMORY; goto cleanup;
    }
    status = UmiAiTrainingAdamWCheckpointEncode(model, optimizer,
                                                 bytes, required, &written);
    if (status != UMI_STATUS_OK) goto cleanup;
    memcpy(stage, path, pathLength);
    memcpy(stage + pathLength, ".umi-adamw-partial", sizeof(".umi-adamw-partial"));
#if defined(_WIN32)
    descriptor = _open(stage, _O_CREAT | _O_EXCL | _O_WRONLY | _O_BINARY,
                       _S_IREAD | _S_IWRITE);
#else
    descriptor = open(stage, O_CREAT | O_EXCL | O_WRONLY, 0600);
#endif
    if (descriptor < 0) {
        status = errno == EEXIST ? UMI_STATUS_ALREADY_EXISTS : UMI_STATUS_IO_ERROR;
        goto cleanup;
    }
    size_t offset = 0U;
    while (offset < written) {
#if defined(_WIN32)
        int n = _write(descriptor, bytes + offset, (unsigned int)(written - offset));
#else
        ssize_t n = write(descriptor, bytes + offset, written - offset);
#endif
        if (n <= 0) { status = UMI_STATUS_IO_ERROR; goto remove_stage; }
        offset += (size_t)n;
    }
#if defined(_WIN32)
    int syncStatus = _commit(descriptor);
    int closeStatus = _close(descriptor);
#else
    int syncStatus = fsync(descriptor);
    int closeStatus = close(descriptor);
#endif
    descriptor = -1;
    if (syncStatus != 0 || closeStatus != 0) {
        status = UMI_STATUS_IO_ERROR; goto remove_stage;
    }
#if defined(_WIN32)
    if (!MoveFileExA(stage, path, MOVEFILE_WRITE_THROUGH)) {
        DWORD error = GetLastError();
        status = (error == ERROR_ALREADY_EXISTS || error == ERROR_FILE_EXISTS) ?
                 UMI_STATUS_ALREADY_EXISTS : UMI_STATUS_IO_ERROR;
        goto remove_stage;
    }
#else
    if (link(stage, path) != 0) {
        status = errno == EEXIST ? UMI_STATUS_ALREADY_EXISTS : UMI_STATUS_IO_ERROR;
        goto remove_stage;
    }
    (void)remove(stage);
#endif
    status = UMI_STATUS_OK;
    goto cleanup;
remove_stage:
#if defined(_WIN32)
    if (descriptor >= 0) (void)_close(descriptor);
#else
    if (descriptor >= 0) (void)close(descriptor);
#endif
    descriptor = -1;
    (void)remove(stage);
cleanup:
    free(stage);
    free(bytes);
    return status;
}

UmiStatus UmiAiTrainingAdamWCheckpointLoadFile(
    const char *path, UmiAiTrainingDecoder **outModel,
    UmiAiTrainingAdamW **outOptimizer)
{
    uint8_t *bytes;
    FILE *stream;
    UmiStatus status;
    long fileLength;
    if (path == NULL || path[0] == '\0' || outModel == NULL || outOptimizer == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    stream = fopen(path, "rb");
    if (stream == NULL) return UMI_STATUS_IO_ERROR;
    if (fseek(stream, 0L, SEEK_END) != 0 || (fileLength = ftell(stream)) < 0L ||
        (unsigned long)fileLength > UMI_AI_ADAMW_MAX_CHECKPOINT_BYTES ||
        fseek(stream, 0L, SEEK_SET) != 0) {
        fclose(stream); return UMI_STATUS_PARSE_ERROR;
    }
    bytes = malloc((size_t)fileLength + 1U);
    if (bytes == NULL) { fclose(stream); return UMI_STATUS_OUT_OF_MEMORY; }
    size_t count = fread(bytes, 1U, (size_t)fileLength, stream);
    int extra = fgetc(stream);
    int hadError = ferror(stream);
    if (fclose(stream) != 0) hadError = 1;
    if (hadError || count != (size_t)fileLength) status = UMI_STATUS_IO_ERROR;
    else if (extra != EOF) status = UMI_STATUS_PARSE_ERROR;
    else status = UmiAiTrainingAdamWCheckpointDecode(bytes, count,
                                                     outModel, outOptimizer);
    free(bytes);
    return status;
}
