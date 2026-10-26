/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_stacked_kv_store.c
 *
 * PURPOSE:
 *   Exercise actual per-layer K/V storage, prefix appends and mathematically mandatory full-layer rollover replacement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/stacked_kv_store.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do {if (!(x)) {fprintf(stderr, "kv store %d: %s\n", __LINE__, #x); return 1;}} while(0)
static UmiAiTrainingStackedKvStoreConfig Configuration(void)
{
    UmiAiTrainingStackedKvStoreConfig c = {0};
    c.decoder = (UmiAiTrainingStackedDecoderConfig){260U,4U,4U,8U,2U,3U,UINT64_C(9)};
    for(size_t i=0U;i<32U;++i) c.checkpointSha256[i]=(uint8_t)(i+1U);
    return c;
}
static void Fill(float *values, size_t count, float base)
{
    for (size_t i=0U;i<count;++i) values[i]=base+(float)i*0.01F;
}
int main(void)
{
    UmiAiTrainingStackedKvStore *store=NULL;
    UmiAiTrainingStackedKvStoreConfig config=Configuration();
    UmiAiTrainingStackedKvStoreInfo info={0};
    float keys[3U*4U*4U]={0}, values[3U*4U*4U]={0};
    float keyRow[4]={0},valueRow[4]={0};
    const size_t first[2]={40U,41U};
    Fill(keys,24U,1.0F); Fill(values,24U,-1.0F);
    CHECK(UmiAiTrainingStackedKvStoreCreate(&config,&store)==UMI_STATUS_OK);
    CHECK(store!=NULL);
    CHECK(UmiAiTrainingStackedKvStorePrefill(store,first,2U,keys,24U,values,24U)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedKvStoreInspect(store,&info)==UMI_STATUS_OK);
    CHECK(info.retainedTokens==2U && info.initialPromptTokens==2U);
    CHECK(info.acceptedTokenLayerRows==6U);
    CHECK(info.reservedKvBytes==384U);
    CHECK(UmiAiTrainingStackedKvStoreRead(store,1U,1U,keyRow,4U,valueRow,4U)==UMI_STATUS_OK);
    for(size_t i=0U;i<4U;++i) {
        CHECK(fabsf(keyRow[i]-keys[12U+i])<1.e-6F);
        CHECK(fabsf(valueRow[i]-values[12U+i])<1.e-6F);
    }
    Fill(keys,12U,2.0F); Fill(values,12U,-2.0F);
    CHECK(UmiAiTrainingStackedKvStoreAppend(store,42U,keys,12U,values,12U)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedKvStoreAppend(store,43U,keys,12U,values,12U)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedKvStoreInspect(store,&info)==UMI_STATUS_OK);
    CHECK(info.retainedTokens==4U && info.singleRowAppends==2U);
    CHECK(info.acceptedTokenLayerRows==12U);
    CHECK(UmiAiTrainingStackedKvStoreAppend(store,44U,keys,12U,values,12U)==UMI_STATUS_INVALID_ARGUMENT);
    /* All row values change on rollover; stale upper-layer histories must
     * never be retained, even when the token IDs shift correctly. */
    const size_t rolled[4]={41U,42U,43U,44U};
    Fill(keys,48U,4.0F);Fill(values,48U,-4.0F);
    CHECK(UmiAiTrainingStackedKvStoreReplaceRolled(store,rolled,4U,
                keys,48U,values,48U)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedKvStoreInspect(store,&info)==UMI_STATUS_OK);
    CHECK(info.retainedTokens==4U && info.fullWindowRebuilds==1U);
    CHECK(info.acceptedTokenLayerRows==24U);
    CHECK(UmiAiTrainingStackedKvStoreRead(store,2U,3U,keyRow,4U,valueRow,4U)==UMI_STATUS_OK);
    for(size_t i=0U;i<4U;++i) {
        CHECK(fabsf(keyRow[i]-keys[44U+i])<1.e-6F);
        CHECK(fabsf(valueRow[i]-values[44U+i])<1.e-6F);
    }
    CHECK(UmiAiTrainingStackedKvStoreCheckIdentity(store,config.checkpointSha256)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedKvStoreReset(store)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedKvStoreInspect(store,&info)==UMI_STATUS_OK);
    CHECK(info.retainedTokens==0U && info.acceptedTokenLayerRows==0U);
    Fill(keys,12U,1.0F);Fill(values,12U,1.0F);
    const size_t restart[1]={49U};
    CHECK(UmiAiTrainingStackedKvStorePrefill(store,restart,1U,keys,12U,values,12U)==UMI_STATUS_OK);
    UmiAiTrainingStackedKvStoreDestroy(store);
    puts("multi-layer KV owner: fixed prefix, layer isolation and full rollover passed");
    return 0;
}
