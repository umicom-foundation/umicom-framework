/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_adamw_resume.c
 * PURPOSE: Prove exact decoder+AdamW moment continuation, byte-for-byte,
 *          against an uninterrupted seeded training run.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------
 * File: tests/ai_training/test_training_adamw_resume.c
 * LICENCE: MIT
 */
#include "umicom/ai/training/optimizer_checkpoint.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #expr); return 1; \
} } while (0)

int main(void)
{
    UmiAiTrainingDecoderConfig cfg = {12U, 3U, 4U, 8U, 2U, 4201U};
    UmiAiTrainingDecoder *reference = NULL, *restored = NULL;
    UmiAiTrainingAdamW *adam = NULL, *resumed = NULL;
    UmiAiTrainingAdamWConfig options;
    size_t tokens[] = {1U, 3U, 5U}, targets[] = {3U, 5U, 7U};
    uint64_t steps = 0U;
    size_t stateBytes = 0U, sizeA = 0U, sizeB = 0U;
    size_t writtenA = 0U, writtenB = 0U;
    uint8_t *bytesA = NULL, *bytesB = NULL;
    float firstLoss = -1.0F, resumedLoss = -1.0F;
    CHECK(UmiAiTrainingAdamWDefaults(&options) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCreate(&cfg, &reference) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderAdamWCreate(reference, &options, &adam)
          == UMI_STATUS_OK);
    for (size_t i = 0U; i < 4U; ++i) {
        CHECK(UmiAiTrainingDecoderTrainAdamWStep(reference, adam,
            tokens, targets, 3U, 0.015F, &firstLoss) == UMI_STATUS_OK);
    }
    CHECK(UmiAiTrainingAdamWCheckpointSize(reference, adam, &sizeA)
          == UMI_STATUS_OK);
    bytesA = malloc(sizeA);
    CHECK(bytesA != NULL);
    CHECK(UmiAiTrainingAdamWCheckpointEncode(reference, adam,
          bytesA, sizeA, &writtenA) == UMI_STATUS_OK);
    CHECK(writtenA == sizeA);
    CHECK(UmiAiTrainingAdamWCheckpointDecode(bytesA, writtenA,
          &restored, &resumed) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingAdamWInspect(resumed, &steps, &stateBytes)
          == UMI_STATUS_OK);
    CHECK(steps == 4U && stateBytes > 0U);
    CHECK(UmiAiTrainingAdamWCheckpointSize(restored, resumed, &sizeB)
          == UMI_STATUS_OK);
    CHECK(sizeB == sizeA);
    bytesB = malloc(sizeB);
    CHECK(bytesB != NULL);
    CHECK(UmiAiTrainingAdamWCheckpointEncode(restored, resumed,
          bytesB, sizeB, &writtenB) == UMI_STATUS_OK);
    CHECK(writtenB == writtenA && memcmp(bytesA, bytesB, sizeA) == 0);
    /* Exactly the same subsequent training sequence and update rates. */
    for (size_t i = 4U; i < 8U; ++i) {
        CHECK(UmiAiTrainingDecoderTrainAdamWStep(reference, adam,
            tokens, targets, 3U, 0.015F, &firstLoss) == UMI_STATUS_OK);
        CHECK(UmiAiTrainingDecoderTrainAdamWStep(restored, resumed,
            tokens, targets, 3U, 0.015F, &resumedLoss) == UMI_STATUS_OK);
        CHECK(isfinite(firstLoss) && firstLoss == resumedLoss);
    }
    CHECK(UmiAiTrainingAdamWCheckpointEncode(reference, adam,
          bytesA, sizeA, &writtenA) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingAdamWCheckpointEncode(restored, resumed,
          bytesB, sizeB, &writtenB) == UMI_STATUS_OK);
    CHECK(writtenA == writtenB && memcmp(bytesA, bytesB, writtenA) == 0);
    UmiAiTrainingAdamWDestroy(resumed);
    UmiAiTrainingAdamWDestroy(adam);
    UmiAiTrainingDecoderDestroy(restored);
    UmiAiTrainingDecoderDestroy(reference);
    free(bytesA); free(bytesB);
    puts("AdamW exact checkpoint resume and model+moments equality: PASS");
    return 0;
}
