/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/checkpoint.c
 *
 * PURPOSE:
 *   Encode/verify experimental decoder checkpoints and publish new local
 *   checkpoint files without overwriting an existing path.
 *
 * FORMAT AND TRUST:
 *   UMIDEC04, version 1, little-endian integers, IEEE754 binary32 values,
 *   fixed parameter order and a trailing FNV-1a64 accidental-corruption check.
 *   FNV is NOT a MAC or digital signature. Never import untrusted artifacts.
 *   Checkpoint size, shape, finiteness and exact EOF are always validated.
 *   Native model bytes are not confused with the older UMIMODEL educational
 *   word-transition format; neither format is silently converted.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab | Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/checkpoint.h"
#include "decoder_private.h"
#include <errno.h>
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

/* Reject platforms lacking the expected binary32 representation. */
#if FLT_RADIX != 2 || FLT_MANT_DIG != 24 || FLT_MAX_EXP != 128
#error Umicom native decoder checkpoints require IEEE754 binary32 float.
#endif
_Static_assert(sizeof(float) == 4U && sizeof(uint32_t) == 4U,
               "binary32 checkpoint representation mismatch");

#define UMICOM_DECODER_CHECKPOINT_MAGIC "UMIDEC04"
#define UMICOM_DECODER_CHECKPOINT_FORMAT 1U
#define UMICOM_DECODER_CHECKPOINT_HEADER 52U
#define UMICOM_DECODER_CHECKPOINT_TRAILER 8U

static void Put32(uint8_t *bytes, size_t *position, uint32_t value)
{
    for (size_t shift = 0U; shift < 4U; ++shift)
        bytes[(*position)++] = (uint8_t)(value >> (8U * shift));
}

static void Put64(uint8_t *bytes, size_t *position, uint64_t value)
{
    for (size_t shift = 0U; shift < 8U; ++shift)
        bytes[(*position)++] = (uint8_t)(value >> (8U * shift));
}

static uint32_t Get32(const uint8_t *bytes, size_t *position)
{
    uint32_t value = 0U;
    for (size_t shift = 0U; shift < 4U; ++shift)
        value |= (uint32_t)bytes[(*position)++] << (8U * shift);
    return value;
}

static uint64_t Get64(const uint8_t *bytes, size_t *position)
{
    uint64_t value = 0U;
    for (size_t shift = 0U; shift < 8U; ++shift)
        value |= (uint64_t)bytes[(*position)++] << (8U * shift);
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

static UmiStatus SizeFromConfig(const UmiAiTrainingDecoderConfig *config,
                                size_t *outSize)
{
    size_t total = UMICOM_DECODER_CHECKPOINT_HEADER +
                   UMICOM_DECODER_CHECKPOINT_TRAILER;
    if (outSize == NULL || UmiAiDecoderValidateConfig(config) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < UMI_DECODER_PARAMETER_COUNT; ++i) {
        size_t rows, columns;
        UmiAiDecoderParameterShape(config, i, &rows, &columns);
        if (columns == 0U || rows > UMI_AI_TRAINING_MAX_ELEMENTS / columns)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        size_t count = rows * columns;
        if (total > UMI_AI_DECODER_MAX_CHECKPOINT_BYTES - 8U - count * 4U)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        total += 8U + count * 4U;
    }
    *outSize = total;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingDecoderCheckpointSize(
    const UmiAiTrainingDecoder *model, size_t *outSize)
{
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return SizeFromConfig(&model->config, outSize);
}

UmiStatus UmiAiTrainingDecoderCheckpointEncode(
    const UmiAiTrainingDecoder *model, uint8_t *outBytes,
    size_t capacity, size_t *outWritten)
{
    size_t required = 0U, position = 0U;
    uint8_t *temporary;
    UmiStatus status;
    if (model == NULL || outBytes == NULL || outWritten == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = SizeFromConfig(&model->config, &required);
    if (status != UMI_STATUS_OK) return status;
    if (required > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    temporary = malloc(required);
    if (temporary == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(temporary, UMICOM_DECODER_CHECKPOINT_MAGIC, 8U);
    position = 8U;
    Put32(temporary, &position, UMICOM_DECODER_CHECKPOINT_FORMAT);
    Put32(temporary, &position, model->config.vocabularySize);
    Put32(temporary, &position, model->config.contextLength);
    Put32(temporary, &position, model->config.channels);
    Put32(temporary, &position, model->config.hiddenChannels);
    Put32(temporary, &position, model->config.attentionHeads);
    Put64(temporary, &position, model->config.initialSeed);
    Put64(temporary, &position, model->completedSteps);
    Put32(temporary, &position, UMI_DECODER_PARAMETER_COUNT);
    for (size_t p = 0U; p < UMI_DECODER_PARAMETER_COUNT; ++p) {
        size_t rows, columns;
        float values[UMI_AI_TRAINING_MAX_ELEMENTS];
        UmiAiDecoderParameterShape(&model->config, p, &rows, &columns);
        size_t count = rows * columns;
        status = UmiAiTrainingTensorValues(model->parameters[p], values, count);
        if (status != UMI_STATUS_OK) { free(temporary); return status; }
        Put32(temporary, &position, (uint32_t)rows);
        Put32(temporary, &position, (uint32_t)columns);
        for (size_t i = 0U; i < count; ++i) {
            uint32_t bits;
            if (!isfinite(values[i])) { free(temporary); return UMI_STATUS_INVALID_STATE; }
            memcpy(&bits, &values[i], sizeof(bits));
            Put32(temporary, &position, bits);
        }
    }
    if (position + UMICOM_DECODER_CHECKPOINT_TRAILER != required) {
        free(temporary);
        return UMI_STATUS_INTERNAL_ERROR;
    }
    Put64(temporary, &position, Checksum(temporary, required - 8U));
    memcpy(outBytes, temporary, required);
    *outWritten = required;
    free(temporary);
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingDecoderCheckpointDecode(
    const uint8_t *bytes, size_t byteCount,
    UmiAiTrainingDecoder **outModel)
{
    UmiAiTrainingDecoderConfig config = {0};
    size_t position = 0U, expected = 0U;
    float *values[UMI_DECODER_PARAMETER_COUNT] = {0};
    UmiStatus status = UMI_STATUS_PARSE_ERROR;
    uint64_t steps;
    if (bytes == NULL || outModel == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (byteCount < UMICOM_DECODER_CHECKPOINT_HEADER + 8U ||
        byteCount > UMI_AI_DECODER_MAX_CHECKPOINT_BYTES)
        return UMI_STATUS_PARSE_ERROR;
    if (memcmp(bytes, UMICOM_DECODER_CHECKPOINT_MAGIC, 8U) != 0)
        return UMI_STATUS_PARSE_ERROR;
    position = 8U;
    if (Get32(bytes, &position) != UMICOM_DECODER_CHECKPOINT_FORMAT)
        return UMI_STATUS_PARSE_ERROR;
    config.vocabularySize = Get32(bytes, &position);
    config.contextLength = Get32(bytes, &position);
    config.channels = Get32(bytes, &position);
    config.hiddenChannels = Get32(bytes, &position);
    config.attentionHeads = Get32(bytes, &position);
    config.initialSeed = Get64(bytes, &position);
    steps = Get64(bytes, &position);
    if (Get32(bytes, &position) != UMI_DECODER_PARAMETER_COUNT)
        return UMI_STATUS_PARSE_ERROR;
    if (SizeFromConfig(&config, &expected) != UMI_STATUS_OK ||
        expected != byteCount) return UMI_STATUS_PARSE_ERROR;
    size_t tail = byteCount - UMICOM_DECODER_CHECKPOINT_TRAILER;
    if (Checksum(bytes, tail) != Get64(bytes, &tail))
        return UMI_STATUS_PARSE_ERROR;

    for (size_t p = 0U; p < UMI_DECODER_PARAMETER_COUNT; ++p) {
        size_t rows, columns;
        UmiAiDecoderParameterShape(&config, p, &rows, &columns);
        if (Get32(bytes, &position) != rows ||
            Get32(bytes, &position) != columns) goto failure;
        size_t count = rows * columns;
        values[p] = malloc(count * sizeof(*values[p]));
        if (values[p] == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto failure; }
        for (size_t i = 0U; i < count; ++i) {
            uint32_t bits = Get32(bytes, &position);
            memcpy(&values[p][i], &bits, sizeof(bits));
            if (!isfinite(values[p][i])) goto failure;
        }
    }
    status = UmiAiDecoderCreateFromValues(&config, (const float *const *)values,
                                          steps, outModel);
failure:
    for (size_t p = 0U; p < UMI_DECODER_PARAMETER_COUNT; ++p) free(values[p]);
    return status;
}

/* Publish only a new file, never replacing an existing model generation.
 * On Linux the hard-link operation is an atomic no-clobber publication;
 * Windows MoveFileEx without REPLACE_EXISTING has the same intention.
 * A crash during staging can leave a *.umicom-partial file; applications may
 * review and remove stale stages. Filesystem power-loss durability is not
 * promised: no directory fsync or transactional filesystem is assumed. */
UmiStatus UmiAiTrainingDecoderCheckpointSaveNewFile(
    const UmiAiTrainingDecoder *model, const char *path)
{
    size_t required = 0U, written = 0U;
    uint8_t *bytes = NULL;
    char *stage = NULL;
    int descriptor = -1;
    UmiStatus status;
    size_t pathLength;
    if (model == NULL || path == NULL || path[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    pathLength = strlen(path);
    if (pathLength > 4000U) return UMI_STATUS_CAPACITY_EXCEEDED;
    status = UmiAiTrainingDecoderCheckpointSize(model, &required);
    if (status != UMI_STATUS_OK) return status;
    bytes = malloc(required);
    stage = malloc(pathLength + sizeof(".umicom-partial"));
    if (bytes == NULL || stage == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto cleanup; }
    status = UmiAiTrainingDecoderCheckpointEncode(model, bytes, required, &written);
    if (status != UMI_STATUS_OK) goto cleanup;
    memcpy(stage, path, pathLength);
    memcpy(stage + pathLength, ".umicom-partial", sizeof(".umicom-partial"));
#if defined(_WIN32)
    descriptor = _open(stage, _O_CREAT | _O_EXCL | _O_WRONLY | _O_BINARY,
                       _S_IREAD | _S_IWRITE);
#else
    descriptor = open(stage, O_CREAT | O_EXCL | O_WRONLY, 0600);
#endif
    if (descriptor < 0) {
        status = (errno == EEXIST) ? UMI_STATUS_ALREADY_EXISTS : UMI_STATUS_IO_ERROR;
        goto cleanup;
    }
    /* Use the native descriptor to write directly without an extra CRT stream
     * or platform-specific buffering, then flush and close before publishing. */
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
    /* We exclusively created this staging path, so it is safe to unlink. */
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

UmiStatus UmiAiTrainingDecoderCheckpointLoadFile(
    const char *path, UmiAiTrainingDecoder **outModel)
{
    uint8_t *bytes;
    FILE *stream;
    UmiStatus status;
    long fileLength;
    if (path == NULL || path[0] == '\0' || outModel == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    stream = fopen(path, "rb");
    if (stream == NULL) return UMI_STATUS_IO_ERROR;
    if (fseek(stream, 0L, SEEK_END) != 0) { fclose(stream); return UMI_STATUS_IO_ERROR; }
    fileLength = ftell(stream);
    if (fileLength < 0 || (unsigned long)fileLength > UMI_AI_DECODER_MAX_CHECKPOINT_BYTES ||
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
    else status = UmiAiTrainingDecoderCheckpointDecode(bytes, count, outModel);
    free(bytes);
    return status;
}
