/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_projection_snapshot.c
 * PURPOSE: Verify exact trainable Q/K/MLP adapter weights and step recovery
 *          against the same real frozen decoder checkpoint.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/decoder_projection_snapshot.h"
#include "umicom/ai/training/supervised_window.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"projection snapshot:%d %s\n",__LINE__,#x); return 1; } } while(0)
int main(void)
{
    const UmiAiTrainingDecoderConfig cfg = {260U,8U,8U,16U,2U,UINT64_C(73)};
    UmiAiTrainingDecoder *decoder = NULL;
    UmiAiTrainingDecoderProjectionAdapter *original = NULL, *restored = NULL;
    UmiAiTrainingDecoderProjectionSnapshotInfo meta = {0};
    UmiAiTrainingDecoderProjectionAdapterInfo observed = {0};
    UmiAiTrainingDecoderProjectionStep report = {0};
    UmiAiTrainingSupervisedWindowInfo win = {0};
    const uint8_t prompt[] = {'Q','1'};
    const uint8_t answer[] = {'A','1'};
    size_t inputs[8] = {0}, targets[8] = {0};
    uint8_t mask[8] = {0};
    float before[8U*260U] = {0}, after[8U*260U] = {0};
    uint8_t *encoded = NULL, *again = NULL;
    size_t required = 0, written = 0, rewritten = 0;
    CHECK(UmiAiTrainingDecoderCreate(&cfg,&decoder)==UMI_STATUS_OK);
    const uint32_t chosen = UMI_AI_DECODER_ADAPT_QUERY |
        UMI_AI_DECODER_ADAPT_KEY | UMI_AI_DECODER_ADAPT_FEED_FORWARD_UP;
    CHECK(UmiAiTrainingDecoderProjectionAdapterCreate(decoder,chosen,2U,19U,
          &original)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingSupervisedByteWindowBuild(prompt,sizeof(prompt),
          answer,sizeof(answer),8U,inputs,8U,targets,8U,mask,8U,&win)==UMI_STATUS_OK);
    for (size_t step=0U;step<3U;++step)
        CHECK(UmiAiTrainingDecoderProjectionAdapterTrainStep(original,
            inputs,targets,mask,8U,0.025F,&report)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderProjectionAdapterLogits(original,inputs,8U,
          before,8U*260U)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderProjectionSnapshotSize(original,&required)==UMI_STATUS_OK);
    CHECK(required>0U && required<UMI_AI_PROJECTION_SNAPSHOT_MAX_BYTES);
    encoded=malloc(required);again=malloc(required);
    CHECK(encoded!=NULL && again!=NULL);
    CHECK(UmiAiTrainingDecoderProjectionSnapshotEncode(original,encoded,
          required,&written)==UMI_STATUS_OK && written==required);
    CHECK(UmiAiTrainingDecoderProjectionSnapshotInspect(encoded,written,
          &meta)==UMI_STATUS_OK);
    CHECK(meta.selectedProjections==chosen && meta.adapterCheckpointCount==3U);
    CHECK(meta.completedAdapterSteps==3U && meta.rank==2U);
    CHECK(UmiAiTrainingDecoderProjectionSnapshotRestore(decoder,encoded,
          written,&restored)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderProjectionAdapterInspect(restored,&observed)==UMI_STATUS_OK);
    CHECK(observed.completedAdapterSteps==3U);
    CHECK(UmiAiTrainingDecoderProjectionAdapterLogits(restored,inputs,8U,
          after,8U*260U)==UMI_STATUS_OK);
    for(size_t i=0U;i<8U*260U;++i)
        CHECK(isfinite(before[i]) && before[i]==after[i]);
    CHECK(UmiAiTrainingDecoderProjectionSnapshotEncode(restored,again,
          required,&rewritten)==UMI_STATUS_OK && rewritten==written);
    CHECK(memcmp(encoded,again,written)==0);
    UmiAiTrainingDecoderProjectionAdapterDestroy(restored);
    UmiAiTrainingDecoderProjectionAdapterDestroy(original);
    UmiAiTrainingDecoderDestroy(decoder);
    free(again); free(encoded);
    puts("Projection snapshot: exact frozen identity, weights and SGD steps restored");
    return 0;
}
