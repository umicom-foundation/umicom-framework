/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: learning/native-llm/prefix-work/audit_rotary_training.c
 * PURPOSE:
 *   Show every checked training step of the existing native rotary-attention
 *   education exercise, with the mathematics explained beside the results.
 *   This executable consumes Framework APIs; it is not a neural-kernel fork.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/rotary_optimisation.h"
#include <math.h>
#include <stdio.h>

int main(void)
{
    const UmiAiTrainingRotaryOptimisationProblem problem = {
        .tokenFeatures={.25F,.50F,.75F,.20F,.40F,.10F,.30F,.80F,
                        .90F,.70F,.20F,.10F},
        .values={.10F,.40F,.70F,.20F,.20F,.10F,.80F,.60F,
                 .90F,.20F,.10F,.50F},
        .desiredAttention={.10F,.40F,.70F,.20F,.50F,.25F,.50F,
                           .65F,.35F,.33F,.50F},
        .queryWeights={1.F,.1F,0.F,0.F,0.F,1.F,.1F,0.F,
                       0.F,0.F,1.F,.1F,.1F,0.F,0.F,1.F},
        .keyWeights={1.F,0.F,.1F,0.F,0.F,1.F,0.F,.1F,
                     .1F,0.F,1.F,0.F,0.F,.1F,0.F,1.F},
        .firstTokenPosition=17U,.frequencyBase=10000.0
    };
    const UmiAiTrainingRotaryOptimisationPlan plan={8U,0.5F};
    UmiAiTrainingRotaryOptimisationResult result = {0};
    UmiStatus status=UmiAiTrainingRotaryOptimisationRun(&problem,&plan,&result);
    if(status!=UMI_STATUS_OK){
        fprintf(stderr,"Training exercise failed with status %d.\n",(int)status);
        return 1;
    }
    if(result.completedSteps!=plan.gradientSteps ||
       !isfinite(result.initialLoss) || !isfinite(result.finalLoss)){
        fputs("Training evidence is incomplete.\n",stderr);
        return 1;
    }
    puts("Umicom Framework: rotary-attention training evidence");
    puts("3 token rows | 2 attention heads | 4 channels | 2 trainable 4x4 matrices");
    puts("Loss is the mean of twelve squared output errors, not model accuracy.");
    printf("Starting loss: %.9f\n",(double)result.initialLoss);
    for(size_t i=0U;i<result.completedSteps;++i)
        printf("After update %zu: %.9f\n",i+1U,(double)result.lossAfterStep[i]);
    printf("Freshly recomputed final loss: %.9f\n",(double)result.finalLoss);
    printf("Observed difference (final - initial): %.9f\n",
           (double)result.finalLoss-(double)result.initialLoss);
    printf("Query gradient magnitude: %.9f\n",(double)result.initialQueryGradientL2);
    printf("Key gradient magnitude: %.9f\n",(double)result.initialKeyGradientL2);
    printf("Peak tape-owned allocations: %zu bytes (not whole-process RAM)\n",
           result.peakTapeBytes);
    puts("This is a small deterministic teaching exercise, not a trained LLM.");
    return 0;
}
