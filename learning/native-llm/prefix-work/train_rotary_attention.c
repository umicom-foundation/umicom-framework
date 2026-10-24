/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: learning/native-llm/prefix-work/train_rotary_attention.c
 * PURPOSE:
 *   Demonstrate one genuinely trainable, small RoPE attention path in native
 *   C23, with measured loss before and after each Framework-owned SGD step.
 *   It is not a decoder, chatbot or production convergence benchmark.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/rotary_optimisation.h"
#include <stdio.h>

int main(void)
{
    const UmiAiTrainingRotaryOptimisationProblem problem = {
        .tokenFeatures = {
            0.25F, 0.50F, 0.75F, 0.20F,
            0.40F, 0.10F, 0.30F, 0.80F,
            0.90F, 0.70F, 0.20F, 0.10F
        },
        .values = {
            0.10F, 0.40F, 0.70F, 0.20F,
            0.20F, 0.10F, 0.80F, 0.60F,
            0.90F, 0.20F, 0.10F, 0.50F
        },
        .desiredAttention = {
            0.10F, 0.40F, 0.70F, 0.20F,
            0.50F, 0.25F, 0.50F, 0.50F,
            0.65F, 0.35F, 0.33F, 0.50F
        },
        .queryWeights = {
            1.00F, 0.10F, 0.00F, 0.00F,
            0.00F, 1.00F, 0.10F, 0.00F,
            0.00F, 0.00F, 1.00F, 0.10F,
            0.10F, 0.00F, 0.00F, 1.00F
        },
        .keyWeights = {
            1.00F, 0.00F, 0.10F, 0.00F,
            0.00F, 1.00F, 0.00F, 0.10F,
            0.10F, 0.00F, 1.00F, 0.00F,
            0.00F, 0.10F, 0.00F, 1.00F
        },
        .firstTokenPosition = 17U,
        .frequencyBase = 10000.0
    };
    const UmiAiTrainingRotaryOptimisationPlan plan = {8U, 0.5F};
    UmiAiTrainingRotaryOptimisationResult result = {0};
    const UmiStatus status = UmiAiTrainingRotaryOptimisationRun(
        &problem, &plan, &result);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "rotary optimisation returned status %d\n", (int)status);
        return 1;
    }
    puts("Umicom Framework — C23 rotary attention training experiment");
    puts("Fixed 3-token / 4-channel example; query/key projection weights trained.");
    printf("Initial MSE (before training):  %.9f\n", (double)result.initialLoss);
    for (size_t i = 0U; i < result.completedSteps; ++i)
        printf("After complete SGD step %zu: %.9f\n",
            i + 1U, (double)result.lossAfterStep[i]);
    printf("Final MSE (fresh forward pass): %.9f\n", (double)result.finalLoss);
    printf("Initial gradient L2 (query/key): %.9f / %.9f\n",
        (double)result.initialQueryGradientL2,
        (double)result.initialKeyGradientL2);
    printf("Observed loss reduction:       %s\n",
        result.lossReduced ? "yes" : "no");
    printf("Tape persistent peak bytes:    %zu\n", result.peakTapeBytes);
    puts("This is not a speedup measurement or a trained autoregressive decoder.");
    return 0;
}
