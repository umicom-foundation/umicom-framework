/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_supervised_quality.c
 * PURPOSE: Evaluate held-out quality acceptance and honest rejected outcomes.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/supervised_quality.h"
#include <stdio.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"quality:%d %s\n",__LINE__,#x); return 1; } } while(0)
int main(void)
{
    UmiAiTrainingSupervisedEvaluation before={
        .exampleCount=2U,.supervisedTokens=10U,.correctTopOneTokens=5U,
        .meanCrossEntropy=2.3,.topOneAccuracy=0.5,
        .perplexity=9.9741824548,.perplexityAvailable=1
    };
    UmiAiTrainingSupervisedEvaluation after=before;
    UmiAiTrainingSupervisedQualityPolicy policy={0.01,0.6,0.2};
    UmiAiTrainingSupervisedQualityReport outcome={0};
    after.correctTopOneTokens=7U;after.topOneAccuracy=0.7;
    after.meanCrossEntropy=1.9;after.perplexity=6.6858944423;
    CHECK(UmiAiTrainingSupervisedQualityEvaluate(&before,&after,
          &policy,&outcome)==UMI_STATUS_OK);
    CHECK(outcome.accepted==1 && outcome.evaluatedTokens==10U);
    CHECK(outcome.lossChange < -0.39 && outcome.accuracyChange>0.19);
    after.meanCrossEntropy=2.38;
    CHECK(UmiAiTrainingSupervisedQualityEvaluate(&before,&after,
          &policy,&outcome)==UMI_STATUS_OK);
    CHECK(outcome.accepted==0 && outcome.lossWithinTolerance==0);
    CHECK(outcome.accuracyAtLeastMinimum==1);
    puts("Quality: accepted and rejected held-out observations distinguished");
    return 0;
}
