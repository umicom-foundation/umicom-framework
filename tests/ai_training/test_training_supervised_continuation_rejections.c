/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_supervised_continuation_rejections.c
 * PURPOSE: Check invalid plan, tampered cursor, snapshot buffer capacity and
 *          output preservation without silently mutating adapter weights.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "continuation_fixture.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if(!(x)){fprintf(stderr,"continuation reject:%d %s\n",__LINE__,#x);return 1;} } while(0)
int main(void)
{
    UmiTestSupervisedFixture fixture;
    UmiAiTrainingDecoder *decoder=NULL;
    UmiAiTrainingSupervisedDataset *dataset=NULL;
    UmiAiTrainingDecoderProjectionAdapter *adapter=NULL;
    UmiAiTrainingSupervisedContinuation state={0},saved={0};
    UmiAiTrainingSupervisedProgress progress={0},sentinel={0};
    const UmiAiTrainingSupervisedExperimentPlan valid={2U,1U,UINT64_C(9),0.02F,0.0};
    UmiAiTrainingSupervisedExperimentPlan bad=valid;
    CHECK(ContinuationCreateFixture(&fixture,&decoder,&dataset,&adapter)==UMI_STATUS_OK);
    memset(&state,0xb3,sizeof(state));
    saved=state;
    bad.learningRate=NAN;
    CHECK(UmiAiTrainingSupervisedContinuationBegin(adapter,dataset,
          &bad,&state)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&state,&saved,sizeof(state))==0);
    CHECK(UmiAiTrainingSupervisedContinuationBegin(adapter,dataset,
          &valid,&state)==UMI_STATUS_OK);
    saved=state;
    memset(&progress,0x7a,sizeof(progress));sentinel=progress;
    ++state.completedAdapterSteps;
    CHECK(UmiAiTrainingSupervisedContinuationTrainNext(adapter,dataset,
          &state,&progress)==UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(&progress,&sentinel,sizeof(progress))==0);
    state=saved;
    uint8_t tiny[8]={0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa};
    size_t written=777U;
    CHECK(UmiAiTrainingSupervisedContinuationSnapshotEncode(adapter,dataset,
          &state,tiny,sizeof(tiny),&written)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(written==777U);
    for(size_t i=0U;i<sizeof(tiny);++i)CHECK(tiny[i]==0xaa);
    CHECK(UmiAiTrainingSupervisedContinuationTrainNext(adapter,dataset,
          &state,&progress)==UMI_STATUS_OK);
    CHECK(progress.positionInEpoch==1U);
    ContinuationDestroyFixture(decoder,dataset,adapter);
    puts("Continuation rejection: bad plan, stale cursor, tiny output refused");
    return 0;
}
