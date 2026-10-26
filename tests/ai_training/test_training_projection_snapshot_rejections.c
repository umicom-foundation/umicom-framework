/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_projection_snapshot_rejections.c
 * PURPOSE: Reject mismatched models, corrupted children, short buffers and
 *          unsupported snapshot formats without publishing partial output.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/decoder_projection_snapshot.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"projection rejection:%d %s\n",__LINE__,#x); return 1; } } while(0)
int main(void)
{
    const UmiAiTrainingDecoderConfig a={260U,8U,8U,16U,2U,UINT64_C(101)};
    const UmiAiTrainingDecoderConfig b={260U,8U,8U,16U,2U,UINT64_C(102)};
    UmiAiTrainingDecoder *modelA=NULL,*modelB=NULL;
    UmiAiTrainingDecoderProjectionAdapter *adapter=NULL,*rejected=NULL;
    UmiAiTrainingDecoderProjectionSnapshotInfo info={0},save={0};
    size_t required=0,written=777U;
    uint8_t sentinel[8]={0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa};
    uint8_t *binary=NULL;
    CHECK(UmiAiTrainingDecoderCreate(&a,&modelA)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCreate(&b,&modelB)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderProjectionAdapterCreate(modelA,
          UMI_AI_DECODER_ADAPT_QUERY|UMI_AI_DECODER_ADAPT_VALUE,2U,3U,
          &adapter)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderProjectionSnapshotSize(adapter,&required)==UMI_STATUS_OK);
    binary=malloc(required);
    CHECK(binary!=NULL);
    CHECK(UmiAiTrainingDecoderProjectionSnapshotEncode(adapter,sentinel,
          sizeof(sentinel),&written)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(written==777U);
    for(size_t i=0U;i<sizeof(sentinel);++i) CHECK(sentinel[i]==0xaa);
    CHECK(UmiAiTrainingDecoderProjectionSnapshotEncode(adapter,binary,
          required,&written)==UMI_STATUS_OK && written==required);
    CHECK(UmiAiTrainingDecoderProjectionSnapshotRestore(modelB,binary,
          required,&rejected)==UMI_STATUS_INVALID_STATE && rejected==NULL);
    CHECK(UmiAiTrainingDecoderProjectionSnapshotInspect(binary,required,
          &info)==UMI_STATUS_OK);
    save=info;
    CHECK(UmiAiTrainingDecoderProjectionSnapshotInspect(binary,required-1U,
          &info)!=UMI_STATUS_OK);
    CHECK(memcmp(&save,&info,sizeof(info))==0);
    binary[required-34U]^=0x11;
    CHECK(UmiAiTrainingDecoderProjectionSnapshotRestore(modelA,binary,
          required,&rejected)!=UMI_STATUS_OK && rejected==NULL);
    binary[required-34U]^=0x11;
    binary[8U]^=0x01;
    CHECK(UmiAiTrainingDecoderProjectionSnapshotInspect(binary,required,
          &info)!=UMI_STATUS_OK);
    CHECK(memcmp(&save,&info,sizeof(info))==0);
    free(binary);
    UmiAiTrainingDecoderProjectionAdapterDestroy(adapter);
    UmiAiTrainingDecoderDestroy(modelB);
    UmiAiTrainingDecoderDestroy(modelA);
    puts("Projection snapshot: corruption, version, capacity, foreign model refused");
    return 0;
}
