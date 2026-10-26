/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_supervised_continuation.c
 * PURPOSE: Qualify exact example cursor, deterministic epochs, weighted loss
 *          and independent held-out assessments on the real native decoder.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "continuation_fixture.h"
#include "umicom/ai/training/checkpoint.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"continuation:%d %s\n",__LINE__,#x); return 1; } } while(0)
int main(void)
{
    UmiTestSupervisedFixture source;
    UmiAiTrainingDecoder *decoder=NULL;
    UmiAiTrainingSupervisedDataset *dataset=NULL;
    UmiAiTrainingDecoderProjectionAdapter *adapter=NULL;
    UmiAiTrainingSupervisedContinuation cursor={0},observed={0};
    UmiAiTrainingSupervisedProgress step={0};
    UmiAiTrainingSupervisedEpochResult epoch={0};
    const UmiAiTrainingSupervisedExperimentPlan plan = {
        3U,1U,UINT64_C(9101),0.018F,0.0
    };
    uint8_t *frozenStart=NULL,*frozenEnd=NULL;
    size_t size=0,firstBytes=0,lastBytes=0;
    CHECK(ContinuationCreateFixture(&source,&decoder,&dataset,&adapter)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCheckpointSize(decoder,&size)==UMI_STATUS_OK);
    frozenStart=malloc(size);frozenEnd=malloc(size);
    CHECK(frozenStart!=NULL && frozenEnd!=NULL);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(decoder,frozenStart,
          size,&firstBytes)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingSupervisedContinuationBegin(adapter,dataset,
          &plan,&cursor)==UMI_STATUS_OK);
    CHECK(cursor.completedEpochs==0U && cursor.nextExampleOffset==0U);
    for(size_t e=0U;e<plan.epochs;++e) {
        for(size_t i=0U;i<2U;++i) {
            CHECK(UmiAiTrainingSupervisedContinuationTrainNext(adapter,dataset,
                  &cursor,&step)==UMI_STATUS_OK);
            CHECK(step.epoch==e+1U && step.positionInEpoch==i+1U);
            CHECK(step.supervisedTokens>0U &&
                  isfinite(step.crossEntropyBeforeUpdate));
            CHECK(UmiAiTrainingSupervisedContinuationInspect(adapter,dataset,
                  &cursor,&observed)==UMI_STATUS_OK);
            CHECK(observed.nextExampleOffset==i+1U);
        }
        CHECK(UmiAiTrainingSupervisedContinuationTrainNext(adapter,dataset,
              &cursor,&step)==UMI_STATUS_INVALID_STATE);
        CHECK(UmiAiTrainingSupervisedContinuationFinishEpoch(adapter,dataset,
              &cursor,&epoch)==UMI_STATUS_OK);
        CHECK(epoch.completedEpochs==e+1U && epoch.heldOutEvaluated==1);
        CHECK(epoch.heldOut.supervisedTokens>0U);
        CHECK(cursor.nextExampleOffset==0U && cursor.completedEpochs==e+1U);
    }
    CHECK(cursor.finished==1 && cursor.completedAdapterSteps==6U);
    CHECK(UmiAiTrainingSupervisedContinuationTrainNext(adapter,dataset,
          &cursor,&step)==UMI_STATUS_INVALID_STATE);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(decoder,frozenEnd,
          size,&lastBytes)==UMI_STATUS_OK);
    CHECK(firstBytes==lastBytes && memcmp(frozenStart,frozenEnd,size)==0);
    ContinuationDestroyFixture(decoder,dataset,adapter);
    free(frozenEnd);free(frozenStart);
    puts("Continuation: seeded per-example SGD, epoch gates and frozen model passed");
    return 0;
}
