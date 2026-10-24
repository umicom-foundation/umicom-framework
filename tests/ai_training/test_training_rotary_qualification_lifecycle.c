/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_rotary_qualification_lifecycle.c
 * PURPOSE:
 *   Stress real tape lifecycle, per-step replay, absolute-position shift,
 *   failure-atomic input rejection and stable published allocation evidence.
 *   Standard Framework CMake links this test to production Umicom::AiTraining.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/rotary_optimisation.h"

#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define REQUIRE(expr) do { if (!(expr)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #expr); \
    return 1; \
} } while (0)

static UmiAiTrainingRotaryOptimisationProblem Input(void)
{
    return (UmiAiTrainingRotaryOptimisationProblem){
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
        .firstTokenPosition=17U, .frequencyBase=10000.0
    };
}

int main(void)
{
    const UmiAiTrainingRotaryOptimisationProblem original = Input();
    const UmiAiTrainingRotaryOptimisationPlan fullPlan = {8U,0.5F};
    UmiAiTrainingRotaryOptimisationResult baseline = {0}, replay = {0};
    size_t trials = 0U;
    REQUIRE(UmiAiTrainingRotaryOptimisationRun(&original,&fullPlan,&baseline)
            == UMI_STATUS_OK);
    REQUIRE(baseline.completedSteps == 8U);
    REQUIRE(baseline.peakTapeBytes != 0U);
    for (size_t iteration = 0U; iteration < 16U; ++iteration) {
        UmiAiTrainingRotaryOptimisationProblem current = original;
        current.firstTokenPosition += (uint64_t)(iteration * 19U);
        const UmiAiTrainingRotaryOptimisationProblem unchanged = current;
        REQUIRE(UmiAiTrainingRotaryOptimisationRun(&current, &fullPlan, &replay)
                == UMI_STATUS_OK);
        REQUIRE(memcmp(&current, &unchanged, sizeof current) == 0);
        REQUIRE(replay.completedSteps == baseline.completedSteps);
        REQUIRE(replay.peakTapeBytes == baseline.peakTapeBytes);
        /* Translating all absolute positions equally preserves their
         * relative differences. Rounding may change a few FP32 ulps. */
        REQUIRE(fabsf(replay.initialLoss - baseline.initialLoss) < 0.000008F);
        REQUIRE(fabsf(replay.finalLoss - baseline.finalLoss) < 0.000008F);
        for(size_t index=0U;index<16U;++index) {
            REQUIRE(fabsf(replay.finalQueryWeights[index]-
                          baseline.finalQueryWeights[index]) < 0.00004F);
            REQUIRE(fabsf(replay.finalKeyWeights[index]-
                          baseline.finalKeyWeights[index]) < 0.00004F);
        }
        for(size_t k=0U;k<8U;++k)
            REQUIRE(fabsf(replay.lossAfterStep[k]-baseline.lossAfterStep[k])
                    < 0.000008F);
        ++trials;
    }
    /* A shorter run must give the exact prefix of the longer trace.
     * This catches graphs accidentally reused with stale post-SGD weights. */
    for (size_t steps = 1U; steps <= 8U; ++steps) {
        const UmiAiTrainingRotaryOptimisationPlan plan = {steps,0.5F};
        UmiAiTrainingRotaryOptimisationResult shortRun = {0};
        REQUIRE(UmiAiTrainingRotaryOptimisationRun(&original,&plan,&shortRun)
                == UMI_STATUS_OK);
        REQUIRE(shortRun.completedSteps == steps);
        REQUIRE(fabsf(shortRun.finalLoss-baseline.lossAfterStep[steps-1U])
                < 0.000002F);
        REQUIRE(fabsf(shortRun.initialLoss-baseline.initialLoss) < 0.000002F);
        ++trials;
    }
    /* Invalid input must not publish partial output. Byte comparison is safe
     * because this buffer was initialised to the same repeated byte pattern. */
    UmiAiTrainingRotaryOptimisationResult sentinel;
    UmiAiTrainingRotaryOptimisationProblem invalid = original;
    unsigned char before[sizeof sentinel];
    memset(&sentinel,0xA5,sizeof sentinel);
    memcpy(before,&sentinel,sizeof sentinel);
    invalid.keyWeights[0] = NAN;
    REQUIRE(UmiAiTrainingRotaryOptimisationRun(&invalid,&fullPlan,&sentinel)
            == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(memcmp(before,&sentinel,sizeof sentinel) == 0);
    REQUIRE(UmiAiTrainingRotaryOptimisationRun(&original,&fullPlan,&replay)
            == UMI_STATUS_OK);
    REQUIRE(memcmp(replay.lossAfterStep,baseline.lossAfterStep,
                   sizeof replay.lossAfterStep) == 0);
    printf("native rotary tape lifecycle: %zu independent runs + reproducible reset + failed input PASS\n",trials);
    return 0;
}
