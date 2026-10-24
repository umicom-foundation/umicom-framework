/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_rotary_training_rejections.c
 * PURPOSE: Enforce finite bounded inputs, position overflow protection,
 *          output alias contract and failure-atomic gradient/tensor output.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/rotary_training.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; \
} } while (0)

int main(void)
{
    UmiAiTrainingRotaryConfig cfg = {4U, 4U, 10000.0};
    float q[4] = {0.1F, 0.2F, 0.3F, 0.4F};
    float k[4] = {0.4F, -0.2F, 0.1F, 0.5F};
    float dq[4], dk[4], saved[4];
    float pair[8];
    const float source[8] = {0.1F,0.2F,0.3F,0.4F,0.5F,0.6F,0.7F,0.8F};
    float tensors[8], savedTensors[8];
    const uint64_t invalidStart[] = {UMI_AI_ROTARY_MAX_POSITION};
    for (size_t i = 0U; i < 4U; ++i) dq[i] = dk[i] = 123.5F;
    memcpy(saved, dq, sizeof saved);
    CHECK(UmiAiTrainingRotaryDotBackward(&cfg, q, k, 4U, 1U, 2U,
        1.0, dq, 4U, dk, 3U) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(UmiAiTrainingRotaryDotBackward(&cfg, q, k, 4U, 1U, 2U,
        NAN, dq, 4U, dk, 4U) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingRotaryDotBackward(&cfg, q, k, 4U,
        UMI_AI_ROTARY_MAX_POSITION + 1U, 2U, 1.0, dq, 4U, dk, 4U)
        == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingRotaryDotBackward(&cfg, q, k, 4U, 1U, 2U,
        1.0e300, dq, 4U, dk, 4U) == UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(dq, saved, sizeof saved) == 0);
    CHECK(memcmp(dk, saved, sizeof saved) == 0);

    /* Equality and partial intersection of two output vectors are refused. */
    CHECK(UmiAiTrainingRotaryDotBackward(&cfg, q, k, 4U, 1U, 2U,
        1.0, dq, 4U, dq, 4U) == UMI_STATUS_INVALID_ARGUMENT);
    for (size_t i = 0U; i < 8U; ++i) pair[i] = 33.0F;
    CHECK(UmiAiTrainingRotaryDotBackward(&cfg, q, k, 4U, 1U, 2U,
        1.0, pair, 4U, pair + 2U, 4U) == UMI_STATUS_INVALID_ARGUMENT);
    for (size_t i = 0U; i < 8U; ++i) CHECK(pair[i] == 33.0F);

    /* Nonfinite input is rejected before any output is published. */
    q[0] = INFINITY;
    CHECK(UmiAiTrainingRotaryDotBackward(&cfg, q, k, 4U, 1U, 2U,
        1.0, dq, 4U, dk, 4U) == UMI_STATUS_INVALID_ARGUMENT);
    q[0] = 0.1F;
    CHECK(memcmp(dq, saved, sizeof saved) == 0);
    CHECK(memcmp(dk, saved, sizeof saved) == 0);

    for (size_t i = 0U; i < 8U; ++i) tensors[i] = 4.25F;
    memcpy(savedTensors, tensors, sizeof tensors);
    CHECK(UmiAiTrainingRotaryTensorForward(&cfg, source, 2U, 2U, 1U,
        invalidStart, 1U, tensors, 8U) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(UmiAiTrainingRotaryTensorForward(&cfg, source, 2U, 2U, 1U,
        NULL, 1U, tensors, 8U) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingRotaryTensorForward(&cfg, source, 2U, 3U, 1U,
        NULL, 0U, tensors, 8U) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingRotaryTensorBackward(&cfg, source, 2U, 2U, 1U,
        NULL, 0U, tensors, 7U) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(UmiAiTrainingRotaryTensorForward(&cfg, source, 2U, 2U, 9U,
        NULL, 0U, tensors, 8U) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(tensors, savedTensors, sizeof tensors) == 0);

    cfg.rotaryChannels = 3U;
    CHECK(UmiAiTrainingRotaryTensorForward(&cfg, source, 2U, 2U, 1U,
        NULL, 0U, tensors, 8U) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(tensors, savedTensors, sizeof tensors) == 0);
    cfg.rotaryChannels = 4U;

    /* Input aliases output in the supported in-place contract. */
    CHECK(UmiAiTrainingRotaryRotateAdjoint(&cfg, k, 4U, 21U,
        k, 4U) == UMI_STATUS_OK);
    puts("Rotary training gradient/tensor atomic rejection contracts PASS");
    return 0;
}
