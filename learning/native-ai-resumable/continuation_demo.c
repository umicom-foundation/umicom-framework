/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: learning/native-ai-resumable/continuation_demo.c
 * PURPOSE:
 *   Teach checkpointable native supervised SGD with fictional public sources,
 *   explicit source admission, a one-example checkpoint and exact restoration.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/supervised_continuation.h"
#include "../../tests/ai_training/supervised_fixture.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    const UmiAiTrainingDecoderConfig cfg={
        260U,8U,8U,16U,2U,UINT64_C(1107)
    };
    const UmiAiTrainingSupervisedExperimentPlan plan={
        2U,1U,UINT64_C(1009),0.018F,0.0
    };
    UmiTestSupervisedFixture fixture;
    UmiAiTrainingDecoder *model=NULL;
    UmiAiTrainingDecoderProjectionAdapter *adapter=NULL,*restored=NULL;
    UmiAiTrainingSupervisedDataset *dataset=NULL;
    UmiAiTrainingSupervisedContinuation cursor={0},continued={0};
    UmiAiTrainingSupervisedProgress step={0};
    UmiAiTrainingSupervisedEpochResult epoch={0};
    uint8_t *encoded=NULL;
    size_t bytes=0,written=0;
    UmiStatus status=UmiTestSupervisedFixturePrepare(&fixture);
    if(status!=UMI_STATUS_OK) goto cleanup;
    status=UmiAiTrainingSupervisedDatasetCreate(fixture.records,
        fixture.lengths,fixture.evidence,4U,8U,&dataset);
    if(status!=UMI_STATUS_OK) goto cleanup;
    status=UmiAiTrainingDecoderCreate(&cfg,&model);
    if(status!=UMI_STATUS_OK) goto cleanup;
    status=UmiAiTrainingDecoderProjectionAdapterCreate(model,
        UMI_AI_DECODER_ADAPT_QUERY|UMI_AI_DECODER_ADAPT_VALUE,
        2U,UINT64_C(11),&adapter);
    if(status!=UMI_STATUS_OK) goto cleanup;
    status=UmiAiTrainingSupervisedContinuationBegin(adapter,dataset,
        &plan,&cursor);
    if(status!=UMI_STATUS_OK) goto cleanup;
    status=UmiAiTrainingSupervisedContinuationTrainNext(adapter,dataset,
        &cursor,&step);
    if(status!=UMI_STATUS_OK) goto cleanup;
    printf("Completed one SGD update; loss before update %.6f\n",
           (double)step.crossEntropyBeforeUpdate);
    status=UmiAiTrainingSupervisedContinuationSnapshotSize(adapter,dataset,
        &cursor,&bytes);
    if(status!=UMI_STATUS_OK) goto cleanup;
    encoded=malloc(bytes);
    if(encoded==NULL){status=UMI_STATUS_OUT_OF_MEMORY;goto cleanup;}
    status=UmiAiTrainingSupervisedContinuationSnapshotEncode(adapter,dataset,
        &cursor,encoded,bytes,&written);
    if(status!=UMI_STATUS_OK) goto cleanup;
    /* Simulate a process boundary: the old adapter is destroyed. The bytes
     * would be persisted through approved Framework storage in a real app. */
    UmiAiTrainingDecoderProjectionAdapterDestroy(adapter);
    adapter=NULL;
    status=UmiAiTrainingSupervisedContinuationSnapshotRestore(model,dataset,
        encoded,written,&restored,&continued);
    if(status!=UMI_STATUS_OK) goto cleanup;
    printf("Recovered cursor: epoch=%zu, next example=%zu, steps=%" PRIu64 "\n",
        continued.completedEpochs+1U,continued.nextExampleOffset,
        continued.completedAdapterSteps);
    while(!continued.finished){
        if(continued.nextExampleOffset < continued.trainingExamples){
            status=UmiAiTrainingSupervisedContinuationTrainNext(restored,
                dataset,&continued,&step);
        }else{
            status=UmiAiTrainingSupervisedContinuationFinishEpoch(restored,
                dataset,&continued,&epoch);
            if(status==UMI_STATUS_OK && epoch.heldOutEvaluated)
                printf("Held-out cross-entropy at epoch %zu: %.6f\n",
                    epoch.completedEpochs,epoch.heldOut.meanCrossEntropy);
        }
        if(status!=UMI_STATUS_OK) goto cleanup;
    }
    printf("Complete: %" PRIu64 " adapter steps; %zu selected targets\n",
        continued.completedAdapterSteps,continued.totalSupervisedTargets);
cleanup:
    UmiAiTrainingDecoderProjectionAdapterDestroy(restored);
    UmiAiTrainingDecoderProjectionAdapterDestroy(adapter);
    UmiAiTrainingDecoderDestroy(model);
    UmiAiTrainingSupervisedDatasetDestroy(dataset);
    free(encoded);
    if(status!=UMI_STATUS_OK){
        fprintf(stderr,"Native continuation demonstration failed (%d)\n",(int)status);
        return 1;
    }
    return 0;
}
