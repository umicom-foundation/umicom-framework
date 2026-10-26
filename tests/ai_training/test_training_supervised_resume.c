/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_supervised_resume.c
 * PURPOSE: Prove in-memory exact restart of adapter SGD after one example,
 *          including identical final snapshots and held-out metrics.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "continuation_fixture.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"resume:%d %s\n",__LINE__,#x); return 1; } } while(0)

static UmiStatus RunToCompletion(
    UmiAiTrainingDecoderProjectionAdapter *adapter,
    UmiAiTrainingSupervisedDataset *dataset,
    UmiAiTrainingSupervisedContinuation *cursor)
{
    while(!cursor->finished) {
        UmiStatus status;
        if(cursor->nextExampleOffset < cursor->trainingExamples) {
            UmiAiTrainingSupervisedProgress progress={0};
            status=UmiAiTrainingSupervisedContinuationTrainNext(adapter,dataset,
                                                                cursor,&progress);
        } else {
            UmiAiTrainingSupervisedEpochResult report={0};
            status=UmiAiTrainingSupervisedContinuationFinishEpoch(adapter,dataset,
                                                                  cursor,&report);
        }
        if(status!=UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

int main(void)
{
    UmiTestSupervisedFixture source;
    UmiAiTrainingDecoder *decoder=NULL;
    UmiAiTrainingSupervisedDataset *dataset=NULL;
    UmiAiTrainingDecoderProjectionAdapter *initial=NULL,*restored=NULL;
    UmiAiTrainingSupervisedContinuation cursor={0}, recovered={0};
    UmiAiTrainingSupervisedProgress progress={0};
    const UmiAiTrainingSupervisedExperimentPlan plan = {
        3U,1U,UINT64_C(88),0.02F,0.0
    };
    uint8_t *checkpoint=NULL,*expected=NULL,*actual=NULL;
    size_t length=0,written=0,expectedBytes=0,actualBytes=0;
    CHECK(ContinuationCreateFixture(&source,&decoder,&dataset,&initial)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingSupervisedContinuationBegin(initial,dataset,
          &plan,&cursor)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingSupervisedContinuationTrainNext(initial,dataset,
          &cursor,&progress)==UMI_STATUS_OK);
    CHECK(cursor.nextExampleOffset==1U && cursor.completedAdapterSteps==1U);
    CHECK(UmiAiTrainingSupervisedContinuationSnapshotSize(initial,dataset,
          &cursor,&length)==UMI_STATUS_OK);
    checkpoint=malloc(length);expected=malloc(length);actual=malloc(length);
    CHECK(checkpoint!=NULL && expected!=NULL && actual!=NULL);
    CHECK(UmiAiTrainingSupervisedContinuationSnapshotEncode(initial,dataset,
          &cursor,checkpoint,length,&written)==UMI_STATUS_OK);
    CHECK(written==length);
    CHECK(UmiAiTrainingSupervisedContinuationSnapshotRestore(decoder,dataset,
          checkpoint,length,&restored,&recovered)==UMI_STATUS_OK);
    CHECK(recovered.nextExampleOffset==cursor.nextExampleOffset);
    CHECK(recovered.completedAdapterSteps==cursor.completedAdapterSteps);
    CHECK(UmiAiTrainingSupervisedContinuationSnapshotEncode(restored,dataset,
          &recovered,actual,length,&actualBytes)==UMI_STATUS_OK);
    CHECK(actualBytes==length && memcmp(checkpoint,actual,length)==0);
    CHECK(RunToCompletion(initial,dataset,&cursor)==UMI_STATUS_OK);
    CHECK(RunToCompletion(restored,dataset,&recovered)==UMI_STATUS_OK);
    CHECK(cursor.finished && recovered.finished);
    CHECK(cursor.completedAdapterSteps==recovered.completedAdapterSteps);
    CHECK(fabs(cursor.lastHeldOut.meanCrossEntropy-
               recovered.lastHeldOut.meanCrossEntropy)<1e-8);
    CHECK(UmiAiTrainingSupervisedContinuationSnapshotEncode(initial,dataset,
          &cursor,expected,length,&expectedBytes)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingSupervisedContinuationSnapshotEncode(restored,dataset,
          &recovered,actual,length,&actualBytes)==UMI_STATUS_OK);
    CHECK(expectedBytes==actualBytes && memcmp(expected,actual,expectedBytes)==0);
    free(actual);free(expected);free(checkpoint);
    UmiAiTrainingDecoderProjectionAdapterDestroy(restored);
    ContinuationDestroyFixture(decoder,dataset,initial);
    puts("Resume: exact mid-epoch weight, cursor, metrics and replay parity verified");
    return 0;
}
