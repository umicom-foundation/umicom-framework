/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_supervised_quality_rejections.c
 * PURPOSE: Refuse unpaired populations, nonfinite thresholds and malformed
 *          held-out evidence without destroying prior caller output.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/supervised_quality.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"quality reject:%d %s\n",__LINE__,#x); return 1; } } while(0)
int main(void)
{
    UmiAiTrainingSupervisedEvaluation before={
        .exampleCount=2U,.supervisedTokens=10U,.correctTopOneTokens=5U,
        .meanCrossEntropy=2.0,.topOneAccuracy=0.5,
        .perplexity=7.389056,.perplexityAvailable=1
    };
    UmiAiTrainingSupervisedEvaluation after=before;
    UmiAiTrainingSupervisedQualityPolicy policy={0.02,0.5,0.0};
    UmiAiTrainingSupervisedQualityReport report;
    memset(&report,0xaa,sizeof(report));
    UmiAiTrainingSupervisedQualityReport original=report;
    after.supervisedTokens=11U;
    CHECK(UmiAiTrainingSupervisedQualityEvaluate(&before,&after,
          &policy,&report)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&report,&original,sizeof(report))==0);
    after=before;after.meanCrossEntropy=NAN;
    CHECK(UmiAiTrainingSupervisedQualityEvaluate(&before,&after,
          &policy,&report)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&report,&original,sizeof(report))==0);
    after=before;policy.minimumTopOneAccuracy=1.1;
    CHECK(UmiAiTrainingSupervisedQualityEvaluate(&before,&after,
          &policy,&report)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&report,&original,sizeof(report))==0);
    puts("Quality: invalid observations and thresholds preserve complete output");
    return 0;
}
