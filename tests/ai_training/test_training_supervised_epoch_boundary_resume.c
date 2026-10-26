/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_supervised_epoch_boundary_resume.c
 * PURPOSE: Resume at the end of an epoch before held-out evaluation without
 *          repeating its final training example.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "continuation_fixture.h"
#include <stdio.h>
#include <stdlib.h>
#define CHECK(x) do { if(!(x)){fprintf(stderr,"epoch boundary:%d %s\n",__LINE__,#x);return 1;} } while(0)
int main(void)
{
    UmiTestSupervisedFixture fixture;
    UmiAiTrainingDecoder *decoder=NULL;
    UmiAiTrainingSupervisedDataset *dataset=NULL;
    UmiAiTrainingDecoderProjectionAdapter *adapter=NULL,*restored=NULL;
    UmiAiTrainingSupervisedContinuation state={0},continued={0};
    UmiAiTrainingSupervisedProgress progress={0};
    UmiAiTrainingSupervisedEpochResult epoch={0};
    const UmiAiTrainingSupervisedExperimentPlan plan={2U,1U,UINT64_C(17),0.02F,0.0};
    uint8_t *bytes=NULL;
    size_t required=0,written=0;
    CHECK(ContinuationCreateFixture(&fixture,&decoder,&dataset,&adapter)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingSupervisedContinuationBegin(adapter,dataset,&plan,
          &state)==UMI_STATUS_OK);
    for(size_t i=0U;i<state.trainingExamples;++i)
        CHECK(UmiAiTrainingSupervisedContinuationTrainNext(adapter,dataset,
            &state,&progress)==UMI_STATUS_OK);
    CHECK(state.nextExampleOffset==state.trainingExamples);
    CHECK(UmiAiTrainingSupervisedContinuationSnapshotSize(adapter,dataset,
          &state,&required)==UMI_STATUS_OK);
    bytes=malloc(required);CHECK(bytes!=NULL);
    CHECK(UmiAiTrainingSupervisedContinuationSnapshotEncode(adapter,dataset,
          &state,bytes,required,&written)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingSupervisedContinuationSnapshotRestore(decoder,dataset,
          bytes,written,&restored,&continued)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingSupervisedContinuationTrainNext(restored,dataset,
          &continued,&progress)==UMI_STATUS_INVALID_STATE);
    CHECK(UmiAiTrainingSupervisedContinuationFinishEpoch(restored,dataset,
          &continued,&epoch)==UMI_STATUS_OK);
    CHECK(continued.completedEpochs==1U && continued.nextExampleOffset==0U &&
          continued.completedAdapterSteps==state.completedAdapterSteps);
    CHECK(UmiAiTrainingSupervisedContinuationTrainNext(restored,dataset,
          &continued,&progress)==UMI_STATUS_OK);
    CHECK(continued.completedAdapterSteps==state.completedAdapterSteps+1U);
    free(bytes);
    UmiAiTrainingDecoderProjectionAdapterDestroy(restored);
    ContinuationDestroyFixture(decoder,dataset,adapter);
    puts("Epoch boundary: evaluation resumed without repeating last training row");
    return 0;
}
