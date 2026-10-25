/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_rotary_window.c
 *
 * PURPOSE:
 *   Independently reconstruct causal single-block sliding attention
 *   and compare it with the Framework-owned RoPE ring-cache results.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_rotary_window.c
 * PURPOSE: Independently reconstruct causal single-block sliding attention
 *          and compare it with the Framework-owned RoPE ring-cache results.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/rotary_window.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; \
} } while (0)

/* A deliberately independent test-only calculation: full-window rotation
 * from absolute token indices on every step, then direct stable softmax.
 * This is not a replacement inference implementation in the Framework. */
static void ReferenceRotate(const float input[4], size_t position,
                            double output[4])
{
    for (size_t pair = 0U; pair < 2U; ++pair) {
        const double theta = (double)position * pow(10000.0, -(double)pair / 2.0);
        const double c = cos(theta), s = sin(theta);
        const size_t index = pair * 2U;
        output[index] = (double)input[index] * c - (double)input[index + 1U] * s;
        output[index + 1U] = (double)input[index] * s + (double)input[index + 1U] * c;
    }
}

static void ReferenceContext(const float query[4],
                             const float keys[18][4],
                             const float values[18][4],
                             size_t position, size_t capacity,
                             float outContext[4])
{
    const size_t first = position + 1U > capacity ? position + 1U - capacity : 0U;
    double q[4] = {0};
    double scores[18] = {0};
    double maxScore = -INFINITY, denominator = 0.0;
    ReferenceRotate(query, position, q);
    for (size_t i = first; i <= position; ++i) {
        double k[4] = {0}, dot = 0.0;
        ReferenceRotate(keys[i], i, k);
        for (size_t ch = 0U; ch < 4U; ++ch) dot += q[ch] * k[ch];
        scores[i] = dot / 2.0; /* sqrt(4) */
        if (scores[i] > maxScore) maxScore = scores[i];
    }
    for (size_t i = first; i <= position; ++i)
        denominator += exp(scores[i] - maxScore);
    for (size_t ch = 0U; ch < 4U; ++ch) {
        double value = 0.0;
        for (size_t i = first; i <= position; ++i)
            value += exp(scores[i] - maxScore) / denominator * (double)values[i][ch];
        outContext[ch] = (float)value;
    }
}

int main(void)
{
    const size_t capacities[] = {1U, 4U, 8U, 16U};
    float keys[18][4] = {{0}};
    float values[18][4] = {{0}};
    float queries[18][4] = {{0}};
    for (size_t i = 0U; i < 18U; ++i) {
        for (size_t ch = 0U; ch < 4U; ++ch) {
            queries[i][ch] = (float)((int)(i * 3U + ch) % 11 - 4) * 0.17F;
            keys[i][ch] = (float)((int)(i + ch * 5U) % 13 - 6) * 0.11F;
            values[i][ch] = (float)((int)(i * 7U + ch) % 17 - 7) * 0.35F;
        }
    }

    for (size_t test = 0U; test < sizeof capacities / sizeof capacities[0]; ++test) {
        const size_t capacity = capacities[test];
        UmiAiTrainingRotaryWindow *window = NULL;
        UmiAiTrainingRotaryWindowStats stats = {0};
        const UmiAiTrainingRotaryWindowConfig config = {
            {4U, 4U, 10000.0}, capacity
        };
        CHECK(UmiAiTrainingRotaryWindowCreate(&config, &window) == UMI_STATUS_OK);
        for (size_t i = 0U; i < 18U; ++i) {
            float actual[4] = {0}, expected[4] = {0};
            ReferenceContext(queries[i], (const float (*)[4])keys,
                             (const float (*)[4])values, i, capacity, expected);
            CHECK(UmiAiTrainingRotaryWindowAppend(window, queries[i], keys[i],
                values[i], 4U, actual, 4U) == UMI_STATUS_OK);
            for (size_t ch = 0U; ch < 4U; ++ch)
                CHECK(fabsf(actual[ch] - expected[ch]) < 0.00002F);
            CHECK(UmiAiTrainingRotaryWindowInspect(window, &stats) == UMI_STATUS_OK);
            CHECK(stats.computedRows == i + 1U);
            CHECK(stats.evictedRows == (i + 1U > capacity ? i + 1U - capacity : 0U));
            CHECK(stats.retainedRows == (i + 1U < capacity ? i + 1U : capacity));
            CHECK(stats.nextAbsolutePosition == i + 1U);
            CHECK(stats.fullWindowRebuilds == 0U);
        }
        CHECK(UmiAiTrainingRotaryWindowReset(window) == UMI_STATUS_OK);
        CHECK(UmiAiTrainingRotaryWindowInspect(window, &stats) == UMI_STATUS_OK);
        CHECK(stats.computedRows == 0U && stats.evictedRows == 0U &&
              stats.retainedRows == 0U && stats.nextAbsolutePosition == 0U);
        UmiAiTrainingRotaryWindowDestroy(window);
    }
    puts("rotary ring: 72 independent full-window comparisons; evictions and reset PASS");
    return 0;
}
