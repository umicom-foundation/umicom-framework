/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_rotary_optimisation_rejections.c
 * PURPOSE: Verify all invalid-argument paths preserve caller inputs and
 *          existing output memory without partial training results.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/rotary_optimisation.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr,"line %d: %s\n",__LINE__,#condition);return 1;\
} } while (0)

int main(void)
{
    UmiAiTrainingRotaryOptimisationProblem input = {0};
    UmiAiTrainingRotaryOptimisationPlan plan = {1U,0.1F};
    UmiAiTrainingRotaryOptimisationResult out;
    unsigned char saved[sizeof out];
    for(size_t i=0U;i<12U;i++) {
        input.tokenFeatures[i] = 0.1F;
        input.values[i] = 0.1F;
        input.desiredAttention[i] = 0.1F;
    }
    for(size_t i=0U;i<16U;i++) {
        input.queryWeights[i] = 0.1F;
        input.keyWeights[i] = 0.1F;
    }
    input.frequencyBase=10000.0;
    memset(&out,0xA5,sizeof out);
    memcpy(saved,&out,sizeof out);
    CHECK(UmiAiTrainingRotaryOptimisationRun(NULL,&plan,&out)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingRotaryOptimisationRun(&input,NULL,&out)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingRotaryOptimisationRun(&input,&plan,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    /* Caller-owned input and result ranges must never alias. Rejection
     * happens before any attempt to write through an output pointer. */
    CHECK(UmiAiTrainingRotaryOptimisationRun(&input,&plan,
        (UmiAiTrainingRotaryOptimisationResult *)(void *)&input)
        == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingRotaryOptimisationRun(&input,&plan,
        (UmiAiTrainingRotaryOptimisationResult *)(void *)&plan)
        == UMI_STATUS_INVALID_ARGUMENT);
    plan.gradientSteps=0U;
    CHECK(UmiAiTrainingRotaryOptimisationRun(&input,&plan,&out)==UMI_STATUS_INVALID_ARGUMENT);
    plan.gradientSteps=UMI_AI_ROTARY_OPTIMISATION_MAX_STEPS+1U;
    CHECK(UmiAiTrainingRotaryOptimisationRun(&input,&plan,&out)==UMI_STATUS_INVALID_ARGUMENT);
    plan.gradientSteps=1U;
    plan.learningRate=0.0F;
    CHECK(UmiAiTrainingRotaryOptimisationRun(&input,&plan,&out)==UMI_STATUS_INVALID_ARGUMENT);
    plan.learningRate=NAN;
    CHECK(UmiAiTrainingRotaryOptimisationRun(&input,&plan,&out)==UMI_STATUS_INVALID_ARGUMENT);
    plan.learningRate=INFINITY;
    CHECK(UmiAiTrainingRotaryOptimisationRun(&input,&plan,&out)==UMI_STATUS_INVALID_ARGUMENT);
    plan.learningRate=1.0001F;
    CHECK(UmiAiTrainingRotaryOptimisationRun(&input,&plan,&out)==UMI_STATUS_INVALID_ARGUMENT);
    plan.learningRate=0.1F;
    input.frequencyBase=1.0;
    CHECK(UmiAiTrainingRotaryOptimisationRun(&input,&plan,&out)==UMI_STATUS_INVALID_ARGUMENT);
    input.frequencyBase=10000.0;
    input.firstTokenPosition=UMI_AI_ROTARY_MAX_POSITION;
    CHECK(UmiAiTrainingRotaryOptimisationRun(&input,&plan,&out)==UMI_STATUS_INVALID_ARGUMENT);
    input.firstTokenPosition=0U;
    input.tokenFeatures[0]=NAN;
    CHECK(UmiAiTrainingRotaryOptimisationRun(&input,&plan,&out)==UMI_STATUS_INVALID_ARGUMENT);
    input.tokenFeatures[0]=0.1F;
    input.keyWeights[14]=INFINITY;
    CHECK(UmiAiTrainingRotaryOptimisationRun(&input,&plan,&out)==UMI_STATUS_INVALID_ARGUMENT);
    input.keyWeights[14]=0.1F;
    CHECK(memcmp(&out,saved,sizeof out)==0);
    puts("rotary optimisation: malformed inputs and failure-atomic output PASS");
    return 0;
}
