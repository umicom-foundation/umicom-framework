/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_initialisation.c
 * PURPOSE: Verify seeded FP32 parameter samples and failure atomicity.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/initialisation.h"
#include "umicom/ai/training/tape.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#define VERIFY(ok) do { if (!(ok)) { \
    fprintf(stderr, "initialisation verification failed, line %d: %s\n", \
        __LINE__, #ok); return 1; } } while (0)

int main(void)
{
    UmiAiTrainingRandomState first = {0};
    UmiAiTrainingRandomState second = {0};
    UmiAiTrainingRandomState other = {0};
    UmiAiTrainingRandomState before;
    float samples[8] = {0};
    float replay[8] = {0};
    float unrelated[8] = {0};
    float sentinel[2] = {-8.0F, -9.0F};
    float oneSample = -123.0F;
    VERIFY(UmiAiTrainingRandomSeed(&first, 0U) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingRandomSeed(&second, 0U) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingRandomSeed(&other, 12345U) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingRandomFillUniform(&first, -0.25F, 0.25F, samples, 8U)
           == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingRandomFillUniform(&second, -0.25F, 0.25F, replay, 8U)
           == UMI_STATUS_OK);
    VERIFY(memcmp(samples, replay, sizeof(samples)) == 0);
    VERIFY(first.state == second.state);
    VERIFY(UmiAiTrainingRandomFillUniform(&other, -0.25F, 0.25F, unrelated, 8U)
           == UMI_STATUS_OK);
    VERIFY(memcmp(samples, unrelated, sizeof(samples)) != 0);
    for (size_t i = 0U; i < 8U; ++i) {
        VERIFY(isfinite(samples[i]));
        VERIFY(samples[i] >= -0.25F && samples[i] <= 0.25F);
    }
    before = first;
    VERIFY(UmiAiTrainingRandomUniform(&first, NAN, 0.25F, &oneSample)
           == UMI_STATUS_INVALID_ARGUMENT);
    VERIFY(first.state == before.state && oneSample == -123.0F);
    VERIFY(UmiAiTrainingRandomUniform(&first, 1.0F, 1.0F, &oneSample)
           == UMI_STATUS_INVALID_ARGUMENT);
    VERIFY(first.state == before.state && oneSample == -123.0F);
    /* The former tensor ceiling is kept for review; a larger model now
     * accepts this many values, while exceeding the current ceiling fails. */
#if 0
    VERIFY(UmiAiTrainingRandomFillUniform(&first, -1.0F, 1.0F, sentinel, 4097U)
           == UMI_STATUS_CAPACITY_EXCEEDED);
#endif
    VERIFY(UmiAiTrainingRandomFillUniform(&first, -1.0F, 1.0F, sentinel,
           UMI_AI_TRAINING_MAX_ELEMENTS + 1U) == UMI_STATUS_CAPACITY_EXCEEDED);
    VERIFY(first.state == before.state);
    VERIFY(sentinel[0] == -8.0F && sentinel[1] == -9.0F);
    VERIFY(UmiAiTrainingRandomFillUniform(&first, -1.0F, 1.0F, NULL, 0U)
           == UMI_STATUS_OK);
    VERIFY(first.state == before.state);
    VERIFY(UmiAiTrainingRandomUniform(&first, -0.5F, 0.5F, &oneSample)
           == UMI_STATUS_OK);
    VERIFY(oneSample >= -0.5F && oneSample <= 0.5F);
    VERIFY(first.state != before.state);
    puts("seeded parameter initialisation verification passed");
    return 0;
}
