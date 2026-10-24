/*-----------------------------------------------------------------------------
 * Umicom Framework — Native LLM Learning Laboratory
 * File: learning/native-llm/prefix-work/inspect_rotary_window.c
 * PURPOSE: Show first-block RoPE ring reuse without claiming existing decoder
 *          compatibility or speedups from a different model family.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/rotary_window.h"

#include <inttypes.h>
#include <stdio.h>

int main(void)
{
    const UmiAiTrainingRotaryWindowConfig config = {
        {4U, 4U, 10000.0}, 4U
    };
    UmiAiTrainingRotaryWindow *window = NULL;
    UmiAiTrainingRotaryWindowStats stats = {0};
    float output[4] = {0};
    UmiStatus status = UmiAiTrainingRotaryWindowCreate(&config, &window);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "rotary window allocation refused: %d\n", (int)status);
        return 1;
    }
    for (size_t i = 0U; i < 12U; ++i) {
        const float q[4] = {1.0F, 0.1F * (float)i, 0.4F, -0.2F};
        const float k[4] = {0.3F, 1.0F, (float)i * 0.02F, 0.7F};
        const float v[4] = {(float)i, 2.0F, -1.0F, 0.5F};
        status = UmiAiTrainingRotaryWindowAppend(
            window, q, k, v, 4U, output, 4U);
        if (status != UMI_STATUS_OK) break;
    }
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingRotaryWindowInspect(window, &stats);
    UmiAiTrainingRotaryWindowDestroy(window);
    if (status != UMI_STATUS_OK || stats.computedRows != 12U ||
        stats.evictedRows != 8U || stats.fullWindowRebuilds != 0U ||
        stats.retainedRows != 4U)
    {
        fprintf(stderr, "rotary study rejected (status=%d)\n", (int)status);
        return 1;
    }
    printf("Umicom C23 RoPE SINGLE-BLOCK research, independent Q/K/V projections\n"
           "Total inserted key/value rows: %" PRIu64 "\n"
           "Oldest rows evicted: %" PRIu64 "\n"
           "Full-window Q/K/V rebuilds: %" PRIu64 "\n"
           "Rows retained: %zu\n"
           "Last attention context[0]: %.6f\n",
           stats.computedRows, stats.evictedRows, stats.fullWindowRebuilds,
           stats.retainedRows, (double)output[0]);
    puts("NOT a trained model; legacy learned-absolute checkpoints remain incompatible.");
    puts("Higher attention layers and full decoder integration require separate proof.");
    return 0;
}
