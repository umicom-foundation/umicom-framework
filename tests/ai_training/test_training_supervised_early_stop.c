/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_supervised_early_stop.c
 * PURPOSE: Prove held-out target stopping occurs at an epoch boundary without
 *          consuming examples from a later epoch.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "continuation_fixture.h"
#include <stdio.h>
#define CHECK(x) do { if(!(x)){fprintf(stderr,"early stop:%d %s\n",__LINE__,#x);return 1;} } while(0)
int main(void)
{
    UmiTestSupervisedFixture fixture;
    UmiAiTrainingDecoder *decoder=NULL;
    UmiAiTrainingSupervisedDataset *dataset=NULL;
    UmiAiTrainingDecoderProjectionAdapter *adapter=NULL;
    UmiAiTrainingSupervisedContinuation state={0};
    UmiAiTrainingSupervisedProgress progress={0};
    UmiAiTrainingSupervisedEpochResult epoch={0};
    /* The generous threshold is chosen only for this fictional fixture. */
    const UmiAiTrainingSupervisedExperimentPlan plan={
        5U,1U,UINT64_C(44),0.012F,1.0e6
    };
    CHECK(ContinuationCreateFixture(&fixture,&decoder,&dataset,&adapter)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingSupervisedContinuationBegin(adapter,dataset,
          &plan,&state)==UMI_STATUS_OK);
    for(size_t i=0U;i<state.trainingExamples;++i)
        CHECK(UmiAiTrainingSupervisedContinuationTrainNext(adapter,dataset,
             &state,&progress)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingSupervisedContinuationFinishEpoch(adapter,dataset,
          &state,&epoch)==UMI_STATUS_OK);
    CHECK(state.completedEpochs==1U && state.finished==1 &&
          state.stoppedAtTarget==1 && epoch.stoppedAtTarget==1);
    CHECK(state.completedAdapterSteps==state.trainingExamples);
    CHECK(UmiAiTrainingSupervisedContinuationTrainNext(adapter,dataset,
          &state,&progress)==UMI_STATUS_INVALID_STATE);
    ContinuationDestroyFixture(decoder,dataset,adapter);
    puts("Early stopping: no extra examples after held-out threshold");
    return 0;
}
