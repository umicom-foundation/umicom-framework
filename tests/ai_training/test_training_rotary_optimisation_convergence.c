/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_rotary_optimisation_convergence.c
 * PURPOSE: Verify deterministic, bounded RoPE/SGD optimisation and independent
 *          forward-loss recomputation after the final parameter update.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/rotary_optimisation.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr,"line %d: %s\n", __LINE__, #condition);return 1;\
} } while (0)

/* Deterministic Umicom-native financial-system feature vectors; these are
 * floating-point teaching features, not monetary accounting amounts. */
static UmiAiTrainingRotaryOptimisationProblem Example(void)
{
    UmiAiTrainingRotaryOptimisationProblem result = {
        .tokenFeatures = {.25F,.50F,.75F,.20F, .40F,.10F,.30F,.80F,
                          .90F,.70F,.20F,.10F},
        .values = {.10F,.40F,.70F,.20F, .20F,.10F,.80F,.60F,
                   .90F,.20F,.10F,.50F},
        .desiredAttention = {.10F,.40F,.70F,.20F, .50F,.25F,.50F,.50F,
                             .65F,.35F,.33F,.50F},
        .queryWeights = {1.F,.1F,0.F,0.F, 0.F,1.F,.1F,0.F,
                         0.F,0.F,1.F,.1F, .1F,0.F,0.F,1.F},
        .keyWeights = {1.F,0.F,.1F,0.F, 0.F,1.F,0.F,.1F,
                       .1F,0.F,1.F,0.F, 0.F,.1F,0.F,1.F},
        .firstTokenPosition = 17U, .frequencyBase = 10000.0
    };
    return result;
}

int main(void)
{
    const UmiAiTrainingRotaryOptimisationProblem input = Example();
    const UmiAiTrainingRotaryOptimisationProblem original = input;
    const UmiAiTrainingRotaryOptimisationPlan plan = {8U,0.5F};
    const UmiAiTrainingRotaryOptimisationPlan one = {1U,0.5F};
    UmiAiTrainingRotaryOptimisationResult a = {0}, b = {0}, single = {0};
    CHECK(UmiAiTrainingRotaryOptimisationRun(&input,&plan,&a) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingRotaryOptimisationRun(&input,&plan,&b) == UMI_STATUS_OK);
    /* Reproducibility compares declared data, not unspecified struct padding. */
    CHECK(a.initialLoss==b.initialLoss && a.finalLoss==b.finalLoss);
    CHECK(a.initialQueryGradientL2==b.initialQueryGradientL2);
    CHECK(a.initialKeyGradientL2==b.initialKeyGradientL2);
    CHECK(a.completedSteps==b.completedSteps && a.peakTapeBytes==b.peakTapeBytes);
    CHECK(memcmp(a.lossAfterStep,b.lossAfterStep,sizeof a.lossAfterStep)==0);
    CHECK(memcmp(a.finalQueryWeights,b.finalQueryWeights,sizeof a.finalQueryWeights)==0);
    CHECK(memcmp(a.finalKeyWeights,b.finalKeyWeights,sizeof a.finalKeyWeights)==0);
    CHECK(memcmp(&input,&original,sizeof input)==0);
    CHECK(a.completedSteps==8U && a.peakTapeBytes>0U);
    CHECK(isfinite(a.initialLoss) && a.initialLoss>0.0F);
    CHECK(isfinite(a.finalLoss) && a.finalLoss>=0.0F);
    CHECK(isfinite(a.initialQueryGradientL2) && a.initialQueryGradientL2>0.0F);
    CHECK(isfinite(a.initialKeyGradientL2) && a.initialKeyGradientL2>0.0F);
    CHECK(a.lossReduced==1);
    CHECK(a.finalLoss<a.initialLoss);
    CHECK(fabsf(a.finalQueryWeights[0]-input.queryWeights[0])>0.0000001F);
    CHECK(fabsf(a.finalKeyWeights[0]-input.keyWeights[0])>0.0000001F);
    CHECK(UmiAiTrainingRotaryOptimisationRun(&input,&one,&single)==UMI_STATUS_OK);
    CHECK(single.completedSteps==1U);
    CHECK(fabsf(single.finalLoss-a.lossAfterStep[0])<=0.000002F);
    for(size_t i=0U;i<a.completedSteps;i++)
        CHECK(isfinite(a.lossAfterStep[i]) && a.lossAfterStep[i]>=0.0F);
    CHECK(fabsf(a.lossAfterStep[7]-a.finalLoss)<0.0000001F);
    puts("rotary optimisation: 8 real SGD steps, reproducible weights, fresh loss PASS");
    return 0;
}
