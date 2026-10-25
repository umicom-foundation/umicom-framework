/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/adapter_checkpoint.c
 * PURPOSE:
 *   Encode and restore bounded, independent trainable adapter weights without
 *   modifying the original Transformer or its checkpoint representation.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/adapter_checkpoint.h"
#include "umicom/base/sha256.h"

#include <float.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define ADAPTER_HEADER_BYTES 56U
#define ADAPTER_TRAILER_BYTES 32U
#define ADAPTER_FORMAT_REVISION 1U
static const unsigned char ADAPTER_MAGIC[8] = {
    'U', 'M', 'I', 'L', 'O', 'R', 'A', '!'
};

/* Explicit byte order protects checkpoints when the host CPU is not little
 * endian. Float bits are copied, never type-punned through an aliasing cast. */
static void AdapterWriteWord(unsigned char *destination, uint32_t word)
{
    for (size_t i = 0U; i < 4U; ++i)
        destination[i] = (unsigned char)(word >> (i * 8U));
}

static uint32_t AdapterReadWord(const unsigned char *source)
{
    uint32_t value = 0U;
    for (size_t i = 0U; i < 4U; ++i)
        value |= (uint32_t)source[i] << (i * 8U);
    return value;
}

static int AdapterFloatSupported(void)
{
    return sizeof(float) == 4U && FLT_RADIX == 2 && FLT_MANT_DIG == 24 &&
           FLT_MAX_EXP == 128;
}

static int AdapterShapeValid(size_t rows, size_t columns)
{
    return rows != 0U && columns != 0U &&
           rows <= UINT32_MAX && columns <= UINT32_MAX &&
           columns <= SIZE_MAX / rows &&
           rows * columns <= UMI_AI_TRAINING_MAX_ELEMENTS;
}

static int AdapterDimensionsValid(size_t input, size_t output, size_t rank)
{
    return AdapterShapeValid(input, output) &&
           AdapterShapeValid(input, rank) &&
           AdapterShapeValid(rank, output);
}

static UmiStatus AdapterShapes(
    const UmiAiTrainingLowRankProjection *projection,
    size_t *input, size_t *output, size_t *rank)
{
    size_t rows = 0U, columns = 0U;
    UmiStatus status;
    if (projection == NULL || projection->baseWeights == NULL ||
        projection->adapterDown == NULL || projection->adapterUp == NULL ||
        input == NULL || output == NULL || rank == NULL ||
        !AdapterDimensionsValid(projection->inputChannels,
                                projection->outputChannels, projection->rank))
        return UMI_STATUS_INVALID_ARGUMENT;

    *input = projection->inputChannels;
    *output = projection->outputChannels;
    *rank = projection->rank;

    status = UmiAiTrainingTensorShape(projection->baseWeights, &rows, &columns);
    if (status != UMI_STATUS_OK || rows != *input || columns != *output)
        return UMI_STATUS_INVALID_STATE;
    status = UmiAiTrainingTensorShape(projection->adapterDown, &rows, &columns);
    if (status != UMI_STATUS_OK || rows != *input || columns != *rank)
        return UMI_STATUS_INVALID_STATE;
    status = UmiAiTrainingTensorShape(projection->adapterUp, &rows, &columns);
    if (status != UMI_STATUS_OK || rows != *rank || columns != *output)
        return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

static UmiStatus AdapterEncodedSize(size_t input, size_t output,
                                    size_t rank, size_t *outSize)
{
    size_t downCount, upCount, totalCount;
    if (!AdapterDimensionsValid(input, output, rank) || outSize == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    downCount = input * rank;
    upCount = rank * output;
    if (downCount > SIZE_MAX - upCount)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    totalCount = downCount + upCount;
    if (totalCount > (SIZE_MAX - ADAPTER_HEADER_BYTES -
                      ADAPTER_TRAILER_BYTES) / 4U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    *outSize = ADAPTER_HEADER_BYTES + totalCount * 4U + ADAPTER_TRAILER_BYTES;
    return UMI_STATUS_OK;
}

static UmiStatus AdapterDigest(const unsigned char *bytes, size_t count,
                               unsigned char digest[UMI_SHA256_BYTES])
{
    UmiSha256 state;
    UmiStatus status;
    UmiSha256Init(&state);
    status = UmiSha256Update(&state, bytes, count);
    if (status == UMI_STATUS_OK)
        status = UmiSha256Final(&state, digest);
    return status;
}

static UmiStatus AdapterBaseDigest(const float *base, size_t count,
                                   unsigned char digest[UMI_SHA256_BYTES])
{
    UmiSha256 state;
    unsigned char encoded[4];
    UmiStatus status;
    UmiSha256Init(&state);
    for (size_t i = 0U; i < count; ++i) {
        uint32_t bits = 0U;
        if (!isfinite(base[i])) return UMI_STATUS_INVALID_ARGUMENT;
        memcpy(&bits, &base[i], sizeof(bits));
        AdapterWriteWord(encoded, bits);
        status = UmiSha256Update(&state, encoded, sizeof(encoded));
        if (status != UMI_STATUS_OK) return status;
    }
    return UmiSha256Final(&state, digest);
}

static UmiStatus AdapterEncodeFloats(const float *values, size_t count,
                                     unsigned char *destination)
{
    for (size_t i = 0U; i < count; ++i) {
        uint32_t bits = 0U;
        if (!isfinite(values[i])) return UMI_STATUS_INVALID_STATE;
        memcpy(&bits, &values[i], sizeof(bits));
        AdapterWriteWord(destination + i * 4U, bits);
    }
    return UMI_STATUS_OK;
}

static UmiStatus AdapterDecodeFloats(const unsigned char *source, size_t count,
                                     float *destination)
{
    for (size_t i = 0U; i < count; ++i) {
        uint32_t bits = AdapterReadWord(source + i * 4U);
        float candidate = 0.0F;
        memcpy(&candidate, &bits, sizeof(candidate));
        if (!isfinite(candidate)) return UMI_STATUS_PARSE_ERROR;
        if (destination != NULL) destination[i] = candidate;
    }
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingAdapterCheckpointSize(
    const UmiAiTrainingLowRankProjection *projection, size_t *outBytes)
{
    size_t input = 0U, output = 0U, rank = 0U, proposed = 0U;
    UmiStatus status;
    if (!AdapterFloatSupported()) return UMI_STATUS_UNAVAILABLE;
    if (outBytes == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = AdapterShapes(projection, &input, &output, &rank);
    if (status != UMI_STATUS_OK) return status;
    status = AdapterEncodedSize(input, output, rank, &proposed);
    if (status == UMI_STATUS_OK) *outBytes = proposed;
    return status;
}

UmiStatus UmiAiTrainingAdapterCheckpointExport(
    const UmiAiTrainingLowRankProjection *projection,
    void *outBytes, size_t capacity, size_t *outBytesWritten)
{
    float *base = NULL, *down = NULL, *up = NULL;
    unsigned char *candidate = NULL;
    size_t input = 0U, output = 0U, rank = 0U;
    size_t encodedSize = 0U, downCount, upCount;
    unsigned char digest[UMI_SHA256_BYTES];
    UmiStatus status;

    if (outBytes == NULL || outBytesWritten == NULL ||
        !AdapterFloatSupported())
        return outBytes == NULL || outBytesWritten == NULL
               ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_UNAVAILABLE;
    status = AdapterShapes(projection, &input, &output, &rank);
    if (status != UMI_STATUS_OK) return status;
    status = AdapterEncodedSize(input, output, rank, &encodedSize);
    if (status != UMI_STATUS_OK) return status;
    if (capacity < encodedSize) return UMI_STATUS_CAPACITY_EXCEEDED;

    downCount = input * rank;
    upCount = rank * output;
    base = malloc(input * output * sizeof(*base));
    down = malloc(downCount * sizeof(*down));
    up = malloc(upCount * sizeof(*up));
    candidate = malloc(encodedSize);
    if (base == NULL || down == NULL || up == NULL || candidate == NULL) {
        status = UMI_STATUS_OUT_OF_MEMORY;
        goto complete;
    }

    status = UmiAiTrainingTensorValues(projection->baseWeights,
                                       base, input * output);
    if (status != UMI_STATUS_OK) goto complete;
    status = UmiAiTrainingTensorValues(projection->adapterDown, down, downCount);
    if (status != UMI_STATUS_OK) goto complete;
    status = UmiAiTrainingTensorValues(projection->adapterUp, up, upCount);
    if (status != UMI_STATUS_OK) goto complete;

    memcpy(candidate, ADAPTER_MAGIC, sizeof(ADAPTER_MAGIC));
    AdapterWriteWord(candidate + 8U, ADAPTER_FORMAT_REVISION);
    AdapterWriteWord(candidate + 12U, (uint32_t)input);
    AdapterWriteWord(candidate + 16U, (uint32_t)output);
    AdapterWriteWord(candidate + 20U, (uint32_t)rank);
    status = AdapterBaseDigest(base, input * output, candidate + 24U);
    if (status != UMI_STATUS_OK) goto complete;
    status = AdapterEncodeFloats(down, downCount, candidate + ADAPTER_HEADER_BYTES);
    if (status != UMI_STATUS_OK) goto complete;
    status = AdapterEncodeFloats(up, upCount,
        candidate + ADAPTER_HEADER_BYTES + downCount * 4U);
    if (status != UMI_STATUS_OK) goto complete;
    status = AdapterDigest(candidate, encodedSize - ADAPTER_TRAILER_BYTES, digest);
    if (status != UMI_STATUS_OK) goto complete;
    memcpy(candidate + encodedSize - ADAPTER_TRAILER_BYTES, digest, sizeof(digest));

    memcpy(outBytes, candidate, encodedSize);
    *outBytesWritten = encodedSize;
    status = UMI_STATUS_OK;
complete:
    free(candidate);
    free(up);
    free(down);
    free(base);
    return status;
}

UmiStatus UmiAiTrainingAdapterCheckpointInspect(
    const void *bytes, size_t byteCount,
    UmiAiTrainingAdapterCheckpointInfo *outInfo)
{
    const unsigned char *encoded = bytes;
    UmiAiTrainingAdapterCheckpointInfo result = {0};
    unsigned char digest[UMI_SHA256_BYTES];
    size_t expected = 0U;
    UmiStatus status;
    if (bytes == NULL || outInfo == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!AdapterFloatSupported()) return UMI_STATUS_UNAVAILABLE;
    if (byteCount < ADAPTER_HEADER_BYTES + ADAPTER_TRAILER_BYTES)
        return UMI_STATUS_PARSE_ERROR;
    if (memcmp(encoded, ADAPTER_MAGIC, sizeof(ADAPTER_MAGIC)) != 0 ||
        AdapterReadWord(encoded + 8U) != ADAPTER_FORMAT_REVISION)
        return UMI_STATUS_PARSE_ERROR;

    result.inputChannels = AdapterReadWord(encoded + 12U);
    result.outputChannels = AdapterReadWord(encoded + 16U);
    result.rank = AdapterReadWord(encoded + 20U);
    status = AdapterEncodedSize(result.inputChannels, result.outputChannels,
                                 result.rank, &expected);
    if (status != UMI_STATUS_OK || byteCount != expected)
        return UMI_STATUS_PARSE_ERROR;
    status = AdapterDigest(encoded, byteCount - ADAPTER_TRAILER_BYTES, digest);
    if (status != UMI_STATUS_OK) return status;
    if (memcmp(digest, encoded + byteCount - ADAPTER_TRAILER_BYTES,
               UMI_SHA256_BYTES) != 0)
        return UMI_STATUS_PARSE_ERROR;
    size_t downCount = result.inputChannels * result.rank;
    size_t upCount = result.rank * result.outputChannels;
    status = AdapterDecodeFloats(encoded + ADAPTER_HEADER_BYTES,
                                 downCount + upCount, NULL);
    if (status != UMI_STATUS_OK) return status;
    result.byteCount = byteCount;
    memcpy(result.baseDigest, encoded + 24U, sizeof(result.baseDigest));
    *outInfo = result;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingAdapterCheckpointImport(
    UmiAiTrainingTape *tape,
    const float *baseWeights, size_t baseValueCount,
    const void *bytes, size_t byteCount,
    UmiAiTrainingLowRankProjection *outProjection)
{
    UmiAiTrainingAdapterCheckpointInfo info = {0};
    UmiAiTrainingLowRankProjection restored = {0};
    const unsigned char *encoded = bytes;
    unsigned char actualBaseDigest[UMI_SHA256_BYTES];
    float *down = NULL, *up = NULL;
    size_t checkpoint = 0U;
    size_t downCount, upCount;
    UmiStatus status;

    if (tape == NULL || baseWeights == NULL || outProjection == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingAdapterCheckpointInspect(bytes, byteCount, &info);
    if (status != UMI_STATUS_OK) return status;
    if (baseValueCount != info.inputChannels * info.outputChannels)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = AdapterBaseDigest(baseWeights, baseValueCount, actualBaseDigest);
    if (status != UMI_STATUS_OK) return status;
    if (memcmp(actualBaseDigest, info.baseDigest,
               sizeof(actualBaseDigest)) != 0)
        return UMI_STATUS_INVALID_STATE;

    downCount = info.inputChannels * info.rank;
    upCount = info.rank * info.outputChannels;
    down = malloc(downCount * sizeof(*down));
    up = malloc(upCount * sizeof(*up));
    if (down == NULL || up == NULL) {
        status = UMI_STATUS_OUT_OF_MEMORY;
        goto complete;
    }
    status = AdapterDecodeFloats(encoded + ADAPTER_HEADER_BYTES,
                                 downCount, down);
    if (status != UMI_STATUS_OK) goto complete;
    status = AdapterDecodeFloats(encoded + ADAPTER_HEADER_BYTES + downCount * 4U,
                                 upCount, up);
    if (status != UMI_STATUS_OK) goto complete;
    status = UmiAiTrainingTapeCheckpoint(tape, &checkpoint);
    if (status != UMI_STATUS_OK) goto complete;
    status = UmiAiTrainingTensorCreate(tape, info.inputChannels,
        info.outputChannels, baseWeights, baseValueCount, 0,
        &restored.baseWeights);
    if (status != UMI_STATUS_OK) goto rollback;
    status = UmiAiTrainingTensorCreate(tape, info.inputChannels,
        info.rank, down, downCount, 1, &restored.adapterDown);
    if (status != UMI_STATUS_OK) goto rollback;
    status = UmiAiTrainingTensorCreate(tape, info.rank,
        info.outputChannels, up, upCount, 1, &restored.adapterUp);
    if (status != UMI_STATUS_OK) goto rollback;

    restored.inputChannels = info.inputChannels;
    restored.outputChannels = info.outputChannels;
    restored.rank = info.rank;
    *outProjection = restored;
    status = UMI_STATUS_OK;
    goto complete;
rollback:
    (void)UmiAiTrainingTapeRollback(tape, checkpoint);
complete:
    free(up);
    free(down);
    return status;
}
