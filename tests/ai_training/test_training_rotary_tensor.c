/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_rotary_tensor.c
 * PURPOSE: Validate multi-head batch forward/adjoint against scalar owner,
 *          in-place calls and the independent transpose inner-product rule.
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
    const UmiAiTrainingRotaryConfig cfg = {4U, 4U, 10000.0};
    const uint64_t starts[] = {0U, UINT64_C(3071)};
    enum { BATCHES = 2, TIMESTEPS = 6, HEADS = 2, CHANNELS = 8,
           ELEMENTS = BATCHES * TIMESTEPS * CHANNELS };
    float projected[ELEMENTS] = {0};
    float upstream[ELEMENTS] = {0};
    float forward[ELEMENTS] = {0};
    float backward[ELEMENTS] = {0};
    float inplace[ELEMENTS] = {0};
    double leftInner = 0.0, rightInner = 0.0;
    for (size_t i = 0U; i < ELEMENTS; ++i) {
        projected[i] = (float)(0.03 * (double)((int)(i % 19U) - 9));
        upstream[i] = (float)(0.02 * (double)((int)(i % 23U) - 11));
    }
    CHECK(UmiAiTrainingRotaryTensorForward(&cfg, projected,
        BATCHES * TIMESTEPS, TIMESTEPS, HEADS, starts, BATCHES,
        forward, ELEMENTS) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingRotaryTensorBackward(&cfg, upstream,
        BATCHES * TIMESTEPS, TIMESTEPS, HEADS, starts, BATCHES,
        backward, ELEMENTS) == UMI_STATUS_OK);

    /* Each tensor head must agree with the single responsible RoPE kernel. */
    for (size_t row = 0U; row < BATCHES * TIMESTEPS; ++row) {
        const uint64_t position = starts[row / TIMESTEPS] + row % TIMESTEPS;
        for (size_t head = 0U; head < HEADS; ++head) {
            const size_t offset = row * CHANNELS + head * cfg.headChannels;
            float expected[4] = {0};
            CHECK(UmiAiTrainingRotaryRotate(&cfg, &projected[offset], 4U,
                position, expected, 4U) == UMI_STATUS_OK);
            CHECK(memcmp(&forward[offset], expected,
                         sizeof expected) == 0);
            CHECK(UmiAiTrainingRotaryRotateAdjoint(&cfg, &upstream[offset],
                4U, position, expected, 4U) == UMI_STATUS_OK);
            CHECK(memcmp(&backward[offset], expected,
                         sizeof expected) == 0);
        }
    }
    for (size_t i = 0U; i < ELEMENTS; ++i) {
        leftInner += (double)forward[i] * (double)upstream[i];
        rightInner += (double)projected[i] * (double)backward[i];
    }
    CHECK(fabs(leftInner - rightInner) < 0.000002);

    memcpy(inplace, projected, sizeof inplace);
    CHECK(UmiAiTrainingRotaryTensorForward(&cfg, inplace,
        BATCHES * TIMESTEPS, TIMESTEPS, HEADS, starts, BATCHES,
        inplace, ELEMENTS) == UMI_STATUS_OK);
    CHECK(memcmp(inplace, forward, sizeof forward) == 0);
    memcpy(inplace, upstream, sizeof inplace);
    CHECK(UmiAiTrainingRotaryTensorBackward(&cfg, inplace,
        BATCHES * TIMESTEPS, TIMESTEPS, HEADS, starts, BATCHES,
        inplace, ELEMENTS) == UMI_STATUS_OK);
    CHECK(memcmp(inplace, backward, sizeof backward) == 0);

    /* NULL start vector means independent sequences start at token zero. */
    CHECK(UmiAiTrainingRotaryTensorForward(&cfg, projected,
        BATCHES * TIMESTEPS, TIMESTEPS, HEADS, NULL, 0U,
        inplace, ELEMENTS) == UMI_STATUS_OK);
    for (size_t head = 0U; head < HEADS; ++head) {
        const size_t offset = BATCHES * TIMESTEPS / 2U * CHANNELS +
                              head * cfg.headChannels;
        float expected[4];
        CHECK(UmiAiTrainingRotaryRotate(&cfg, &projected[offset], 4U,
                0U, expected, 4U) == UMI_STATUS_OK);
        CHECK(memcmp(expected, &inplace[offset], sizeof expected) == 0);
    }
    puts("Rotary [batch,time,heads,channels] forward/adjoint identity PASS");
    return 0;
}
