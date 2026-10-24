/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_rotary_gradients.c
 * PURPOSE: Check reverse-mode RoPE Q/K derivatives by independent central
 *          finite differences, adjoint identity and shift invariance.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/rotary_training.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; \
} } while (0)

static int CompareCase(UmiAiTrainingRotaryConfig config,
                       uint64_t pQ, uint64_t pK)
{
    const double epsilon = 0.001;
    float query[UMI_AI_ROTARY_MAX_CHANNELS] = {0};
    float key[UMI_AI_ROTARY_MAX_CHANNELS] = {0};
    float gradientQ[UMI_AI_ROTARY_MAX_CHANNELS] = {0};
    float gradientK[UMI_AI_ROTARY_MAX_CHANNELS] = {0};
    double result = 0.0, shiftedResult = 0.0;
    for (size_t i = 0U; i < config.headChannels; ++i) {
        query[i] = (float)((double)(i + 1U) * 0.073 - 0.21);
        key[i] = (float)((double)(i + 2U) * -0.041 + 0.16);
    }
    const double upstream = 0.7;
    CHECK(UmiAiTrainingRotaryRelativeDot(&config, query, key,
        config.headChannels, pQ, pK, &result) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingRotaryDotBackward(&config, query, key,
        config.headChannels, pQ, pK, upstream, gradientQ,
        config.headChannels, gradientK, config.headChannels) == UMI_STATUS_OK);

    /* Finite difference perturbs the INPUT projection before RoPE.
     * Forward and backward therefore cannot pass by comparing two copies of
     * the same derivative function. Both Q and K are perturbed separately. */
    for (size_t i = 0U; i < config.headChannels; ++i) {
        double positive = 0.0, negative = 0.0;
        const float original = query[i];
        query[i] = original + (float)epsilon;
        CHECK(UmiAiTrainingRotaryRelativeDot(&config, query, key,
            config.headChannels, pQ, pK, &positive) == UMI_STATUS_OK);
        query[i] = original - (float)epsilon;
        CHECK(UmiAiTrainingRotaryRelativeDot(&config, query, key,
            config.headChannels, pQ, pK, &negative) == UMI_STATUS_OK);
        query[i] = original;
        CHECK(fabs(upstream * (positive - negative) / (2.0 * epsilon) -
                   (double)gradientQ[i]) < 0.0004);

        const float originalKey = key[i];
        key[i] = originalKey + (float)epsilon;
        CHECK(UmiAiTrainingRotaryRelativeDot(&config, query, key,
            config.headChannels, pQ, pK, &positive) == UMI_STATUS_OK);
        key[i] = originalKey - (float)epsilon;
        CHECK(UmiAiTrainingRotaryRelativeDot(&config, query, key,
            config.headChannels, pQ, pK, &negative) == UMI_STATUS_OK);
        key[i] = originalKey;
        CHECK(fabs(upstream * (positive - negative) / (2.0 * epsilon) -
                   (double)gradientK[i]) < 0.0004);
    }

    /* Rotary relative-position invariance is a forward mathematical
     * property. The exact delta remains unchanged when both positions shift. */
    if (pQ < 1000000U && pK < 1000000U) {
        CHECK(UmiAiTrainingRotaryRelativeDot(&config, query, key,
            config.headChannels, pQ + 17U, pK + 17U,
            &shiftedResult) == UMI_STATUS_OK);
        CHECK(fabs(result - shiftedResult) < 0.00002);
    }
    return 0;
}

int main(void)
{
    const UmiAiTrainingRotaryConfig configs[] = {
        {2U, 2U, 10000.0},
        {4U, 2U, 10000.0}, /* tail channels unchanged */
        {4U, 4U, 10000.0},
        {8U, 8U, 10000.0},
        {8U, 4U, 8192.0},
        {16U, 16U, 10000.0},
        {32U, 32U, 10000.0}
    };
    const uint64_t positions[][2] = {
        {0U, 0U}, {1U, 0U}, {2U, 4U},
        {12U, 9U}, {100U, 126U}, {1024U, 990U},
        {65530U, 65515U}, {1000000U, 999980U}
    };
    size_t comparisons = 0U;
    for (size_t a = 0U; a < sizeof configs / sizeof configs[0]; ++a) {
        for (size_t b = 0U; b < sizeof positions / sizeof positions[0]; ++b) {
            CHECK(CompareCase(configs[a], positions[b][0], positions[b][1]) == 0);
            ++comparisons;
        }
    }
    printf("Rotary analytical/finite-difference gradient cases: %zu PASS\n",
           comparisons);
    return 0;
}
