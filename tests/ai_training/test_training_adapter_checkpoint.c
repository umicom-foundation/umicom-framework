/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_adapter_checkpoint.c
 * PURPOSE: Verify portable low-rank snapshots, exact base binding, rejection
 *          of corrupted data and atomic restoration of trainable leaves.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/adapter_checkpoint.h"
#include "umicom/base/sha256.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "Adapter checkpoint check failed at line %d: %s\n", \
                __LINE__, #condition); \
        return 1; \
    } \
} while (0)

int main(void)
{
    UmiAiTrainingTape *originalTape = NULL, *restoredTape = NULL;
    UmiAiTrainingLowRankProjection original = {0}, restored = {0};
    UmiAiTrainingAdapterCheckpointInfo info = {0};
    unsigned char *snapshot = NULL, *damaged = NULL;
    unsigned char checksum[UMI_SHA256_BYTES];
    size_t expectedSize = 0U, actualSize = 999U;
    const float base[] = {1.0F, 0.5F, -0.25F, 2.0F,
                          1.5F, -1.0F};
    const float down[] = {0.25F, -0.5F, 0.75F};
    const float up[] = {-0.125F, 0.625F};
    const float wrongBase[] = {1.0F, 0.5F, -0.25F, 2.0F,
                               1.5F, -0.9F};
    float copiedDown[3] = {0}, copiedUp[2] = {0};
    float copiedBase[6] = {0};
    UmiAiTrainingLowRankProjection unchanged = {0};

    CHECK(UmiAiTrainingTapeCreate(&originalTape) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(originalTape, 3U, 2U, base, 6U, 0,
        &original.baseWeights) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(originalTape, 3U, 1U, down, 3U, 1,
        &original.adapterDown) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(originalTape, 1U, 2U, up, 2U, 1,
        &original.adapterUp) == UMI_STATUS_OK);
    original.inputChannels = 3U;
    original.outputChannels = 2U;
    original.rank = 1U;

    CHECK(UmiAiTrainingAdapterCheckpointSize(&original, &expectedSize) == UMI_STATUS_OK);
    CHECK(expectedSize == 56U + (3U + 2U) * 4U + 32U);
    snapshot = malloc(expectedSize);
    damaged = malloc(expectedSize);
    CHECK(snapshot != NULL && damaged != NULL);
    memset(snapshot, 0xa5, expectedSize);
    CHECK(UmiAiTrainingAdapterCheckpointExport(&original,
        snapshot, expectedSize - 1U, &actualSize) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(actualSize == 999U && snapshot[0] == 0xa5U);
    CHECK(UmiAiTrainingAdapterCheckpointExport(&original,
        snapshot, expectedSize, &actualSize) == UMI_STATUS_OK);
    CHECK(actualSize == expectedSize);
    CHECK(UmiAiTrainingAdapterCheckpointInspect(snapshot,
        expectedSize, &info) == UMI_STATUS_OK);
    CHECK(info.inputChannels == 3U && info.outputChannels == 2U &&
          info.rank == 1U && info.byteCount == expectedSize);

    CHECK(UmiAiTrainingTapeCreate(&restoredTape) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingAdapterCheckpointImport(restoredTape, base, 6U,
        snapshot, expectedSize, &restored) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorValues(restored.baseWeights,
        copiedBase, 6U) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorValues(restored.adapterDown,
        copiedDown, 3U) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorValues(restored.adapterUp,
        copiedUp, 2U) == UMI_STATUS_OK);
    CHECK(memcmp(copiedBase, base, sizeof(base)) == 0);
    CHECK(memcmp(copiedDown, down, sizeof(down)) == 0);
    CHECK(memcmp(copiedUp, up, sizeof(up)) == 0);

    /* Check that a mismatching base or manipulated payload cannot publish any
     * new restored handles. A sentinel must survive a failed import. */
    unchanged = restored;
    CHECK(UmiAiTrainingAdapterCheckpointImport(restoredTape, wrongBase, 6U,
        snapshot, expectedSize, &unchanged) == UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(&unchanged, &restored, sizeof(restored)) == 0);
    CHECK(UmiAiTrainingAdapterCheckpointImport(restoredTape, base, 5U,
        snapshot, expectedSize, &unchanged) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&unchanged, &restored, sizeof(restored)) == 0);

    memcpy(damaged, snapshot, expectedSize);
    damaged[56U] ^= 0x40U;
    CHECK(UmiAiTrainingAdapterCheckpointInspect(damaged,
        expectedSize, &info) == UMI_STATUS_PARSE_ERROR);
    CHECK(UmiAiTrainingAdapterCheckpointInspect(damaged,
        expectedSize - 1U, &info) == UMI_STATUS_PARSE_ERROR);
    memcpy(damaged, snapshot, expectedSize);
    damaged[8U] = 0xffU;
    CHECK(UmiAiTrainingAdapterCheckpointInspect(damaged,
        expectedSize, &info) == UMI_STATUS_PARSE_ERROR);

    /* An internally consistent checksum is not enough: non-finite adapter
     * floats must also be rejected before any trainable leaf is created. */
    memcpy(damaged, snapshot, expectedSize);
    damaged[56U] = 0x00U;
    damaged[57U] = 0x00U;
    damaged[58U] = 0xc0U;
    damaged[59U] = 0x7fU; /* A quiet NaN, encoded as little-endian FP32. */
    UmiSha256 sha;
    UmiSha256Init(&sha);
    CHECK(UmiSha256Update(&sha, damaged,
        expectedSize - UMI_SHA256_BYTES) == UMI_STATUS_OK);
    CHECK(UmiSha256Final(&sha, checksum) == UMI_STATUS_OK);
    memcpy(damaged + expectedSize - UMI_SHA256_BYTES,
           checksum, UMI_SHA256_BYTES);
    CHECK(UmiAiTrainingAdapterCheckpointInspect(damaged,
        expectedSize, &info) == UMI_STATUS_PARSE_ERROR);
    CHECK(UmiAiTrainingAdapterCheckpointImport(restoredTape, base, 6U,
        damaged, expectedSize, &unchanged) == UMI_STATUS_PARSE_ERROR);
    CHECK(memcmp(&unchanged, &restored, sizeof(restored)) == 0);

    UmiAiTrainingTapeDestroy(restoredTape);
    UmiAiTrainingTapeDestroy(originalTape);
    free(damaged);
    free(snapshot);
    puts("Verified portable adapter checkpoint, exact base binding and rejection safety.");
    return 0;
}
