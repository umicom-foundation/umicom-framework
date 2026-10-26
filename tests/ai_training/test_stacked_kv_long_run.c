/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_stacked_kv_long_run.c
 * PURPOSE: Validate 500 explicit full-layer rollovers, bounded allocation,
 *          stable row addressing and reproducible reset without decoder math.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation | LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/stacked_kv_store.h"
#include <math.h>
#include <stdio.h>
#define CHECK(x) do {if (!(x)) {fprintf(stderr,"kv long run %d: %s\n",__LINE__,#x);return 1;}} while(0)
int main(void)
{
    UmiAiTrainingStackedKvStoreConfig c={0};
    c.decoder=(UmiAiTrainingStackedDecoderConfig){260U,4U,4U,8U,2U,3U,UINT64_C(19)};
    for(size_t i=0;i<32U;++i)c.checkpointSha256[i]=(uint8_t)(i+1U);
    UmiAiTrainingStackedKvStore *s=NULL;
    CHECK(UmiAiTrainingStackedKvStoreCreate(&c,&s)==UMI_STATUS_OK);
    size_t tokens[4]={10U,11U,12U,13U};
    float keys[48]={0},values[48]={0},outK[4]={0},outV[4]={0};
    CHECK(UmiAiTrainingStackedKvStorePrefill(s,tokens,4U,keys,48U,values,48U)==UMI_STATUS_OK);
    for(size_t step=0;step<500U;++step){
        tokens[0]=tokens[1];tokens[1]=tokens[2];tokens[2]=tokens[3];
        tokens[3]=4U+((step+10U)%256U);
        for(size_t i=0U;i<48U;++i){
            keys[i]=(float)step+(float)i/100.0F;
            values[i]=-(float)step+(float)i/100.0F;
        }
        CHECK(UmiAiTrainingStackedKvStoreReplaceRolled(s,tokens,4U,keys,48U,values,48U)==UMI_STATUS_OK);
        CHECK(UmiAiTrainingStackedKvStoreRead(s,2U,3U,outK,4U,outV,4U)==UMI_STATUS_OK);
        CHECK(fabsf(outK[0]-keys[44U])<0.00005F);
        CHECK(fabsf(outV[0]-values[44U])<0.00005F);
    }
    UmiAiTrainingStackedKvStoreInfo info={0};
    CHECK(UmiAiTrainingStackedKvStoreInspect(s,&info)==UMI_STATUS_OK);
    CHECK(info.fullWindowRebuilds==500U);
    CHECK(info.singleRowAppends==0U);
    CHECK(info.acceptedTokenLayerRows==6012U);
    CHECK(info.reservedKvBytes==384U);
    CHECK(UmiAiTrainingStackedKvStoreReset(s)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedKvStoreInspect(s,&info)==UMI_STATUS_OK);
    CHECK(info.acceptedTokenLayerRows==0U && info.retainedTokens==0U);
    UmiAiTrainingStackedKvStoreDestroy(s);
    puts("500 full-layer KV rollovers passed");
    return 0;
}
