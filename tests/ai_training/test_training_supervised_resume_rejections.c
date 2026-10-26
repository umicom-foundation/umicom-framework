/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_supervised_resume_rejections.c
 * PURPOSE: Preserve existing active state when a snapshot is truncated,
 *          tampered or bound to another approved corpus or frozen model.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "continuation_fixture.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"resume rejections:%d %s\n",__LINE__,#x); return 1; } } while(0)
int main(void)
{
    UmiTestSupervisedFixture source;
    UmiAiTrainingDecoder *decoder=NULL,*foreign=NULL;
    UmiAiTrainingSupervisedDataset *dataset=NULL,*reordered=NULL;
    UmiAiTrainingDecoderProjectionAdapter *adapter=NULL,*invalid=NULL;
    UmiAiTrainingSupervisedContinuation cursor={0},unchanged={0};
    UmiAiTrainingSupervisedProgress progress={0};
    const UmiAiTrainingSupervisedExperimentPlan plan={2U,1U,9U,0.025F,0.0};
    const UmiAiTrainingDecoderConfig foreignConfig={
        260U,8U,8U,16U,2U,UINT64_C(13001)};
    uint8_t *bytes=NULL;
    size_t capacity=0,written=12345U;
    CHECK(ContinuationCreateFixture(&source,&decoder,&dataset,&adapter)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingSupervisedContinuationBegin(adapter,dataset,
          &plan,&cursor)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingSupervisedContinuationTrainNext(adapter,dataset,
          &cursor,&progress)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingSupervisedContinuationSnapshotSize(adapter,dataset,
          &cursor,&capacity)==UMI_STATUS_OK);
    bytes=malloc(capacity);CHECK(bytes!=NULL);
    CHECK(UmiAiTrainingSupervisedContinuationSnapshotEncode(adapter,dataset,
          &cursor,bytes,capacity,&written)==UMI_STATUS_OK && written==capacity);
    memset(&unchanged,0x5a,sizeof(unchanged));
    UmiAiTrainingSupervisedContinuation saved=unchanged;
    CHECK(UmiAiTrainingSupervisedContinuationSnapshotRestore(decoder,dataset,
          bytes,capacity-1U,&invalid,&unchanged)!=UMI_STATUS_OK);
    CHECK(invalid==NULL && memcmp(&unchanged,&saved,sizeof(unchanged))==0);
    bytes[180U]^=0x01;
    CHECK(UmiAiTrainingSupervisedContinuationSnapshotRestore(decoder,dataset,
          bytes,capacity,&invalid,&unchanged)!=UMI_STATUS_OK);
    CHECK(invalid==NULL && memcmp(&unchanged,&saved,sizeof(unchanged))==0);
    bytes[180U]^=0x01;
    CHECK(UmiAiTrainingDecoderCreate(&foreignConfig,&foreign)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingSupervisedContinuationSnapshotRestore(foreign,dataset,
          bytes,capacity,&invalid,&unchanged)!=UMI_STATUS_OK);
    CHECK(invalid==NULL && memcmp(&unchanged,&saved,sizeof(unchanged))==0);
    /* Reordering approved source records changes the canonical source digest,
     * even if the same text is still present in the two partitions. */
    const char *reorderedText[4]={source.records[1],source.records[0],
                                  source.records[2],source.records[3]};
    const size_t reorderedSizes[4]={source.lengths[1],source.lengths[0],
                                    source.lengths[2],source.lengths[3]};
    UmiAiTrainingReviewedExampleEvidence evidence[4]={
        source.evidence[1],source.evidence[0],source.evidence[2],source.evidence[3]
    };
    CHECK(UmiAiTrainingSupervisedDatasetCreate(reorderedText,reorderedSizes,
          evidence,4U,8U,&reordered)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingSupervisedContinuationSnapshotRestore(decoder,reordered,
          bytes,capacity,&invalid,&unchanged)==UMI_STATUS_INVALID_STATE);
    CHECK(invalid==NULL && memcmp(&unchanged,&saved,sizeof(unchanged))==0);
    UmiAiTrainingSupervisedDatasetDestroy(reordered);
    UmiAiTrainingDecoderDestroy(foreign);
    free(bytes);
    ContinuationDestroyFixture(decoder,dataset,adapter);
    puts("Resume: incomplete, tampered, foreign-model and foreign-corpus files rejected");
    return 0;
}
