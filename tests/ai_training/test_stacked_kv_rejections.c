/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_stacked_kv_rejections.c
 * PURPOSE: Test failure-atomic K/V ownership and shape/identity rejection.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation | LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/stacked_kv_store.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do {if (!(x)) {fprintf(stderr, "kv rejections %d: %s\n", __LINE__, #x); return 1;}} while(0)
static UmiAiTrainingStackedKvStoreConfig Configuration(void)
{
    UmiAiTrainingStackedKvStoreConfig c = {0};
    c.decoder=(UmiAiTrainingStackedDecoderConfig){260U,3U,4U,8U,2U,2U,UINT64_C(11)};
    for(size_t i=0U;i<32U;++i)c.checkpointSha256[i]=(uint8_t)(i+1U);
    return c;
}
int main(void)
{
    UmiAiTrainingStackedKvStore *store=NULL;
    UmiAiTrainingStackedKvStoreConfig conf=Configuration();
    UmiAiTrainingStackedKvStoreInfo before={0}, after={0};
    float keys[24],values[24],oldKey[4],oldValue[4];
    const size_t initial[3]={14U,15U,16U};
    for(size_t i=0U;i<24U;++i) {keys[i]=0.1F+(float)i*0.03F;values[i]=-0.2F+(float)i*0.04F;}
    UmiAiTrainingStackedKvStoreConfig invalid=conf;
    invalid.decoder.attentionHeads=3U;
    CHECK(UmiAiTrainingStackedKvStoreCreate(&invalid,&store)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(store==NULL);
    memset(invalid.checkpointSha256,0,sizeof(invalid.checkpointSha256));
    invalid.decoder.attentionHeads=2U;
    CHECK(UmiAiTrainingStackedKvStoreCreate(&invalid,&store)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(store==NULL);
    CHECK(UmiAiTrainingStackedKvStoreCreate(&conf,&store)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedKvStoreAppend(store,17U,keys,8U,values,8U)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingStackedKvStorePrefill(store,initial,3U,keys,23U,values,24U)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingStackedKvStorePrefill(store,initial,3U,keys,24U,values,24U)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedKvStoreInspect(store,&before)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedKvStoreRead(store,1U,0U,oldKey,4U,oldValue,4U)==UMI_STATUS_OK);
    size_t badShift[3]={99U,16U,17U};
    CHECK(UmiAiTrainingStackedKvStoreReplaceRolled(store,badShift,3U,keys,24U,values,24U)==UMI_STATUS_INVALID_STATE);
    CHECK(UmiAiTrainingStackedKvStoreInspect(store,&after)==UMI_STATUS_OK);
    CHECK(memcmp(&before,&after,sizeof(before))==0);
    size_t validShift[3]={15U,16U,17U};
    const float saved=keys[9];keys[9]=NAN;
    CHECK(UmiAiTrainingStackedKvStoreReplaceRolled(store,validShift,3U,keys,24U,values,24U)==UMI_STATUS_INVALID_ARGUMENT);
    keys[9]=saved;
    CHECK(UmiAiTrainingStackedKvStoreInspect(store,&after)==UMI_STATUS_OK);
    CHECK(memcmp(&before,&after,sizeof(before))==0);
    float witnessKey[4],witnessValue[4];
    CHECK(UmiAiTrainingStackedKvStoreRead(store,1U,0U,witnessKey,4U,witnessValue,4U)==UMI_STATUS_OK);
    CHECK(memcmp(witnessKey,oldKey,sizeof(witnessKey))==0);
    CHECK(memcmp(witnessValue,oldValue,sizeof(witnessValue))==0);
    uint8_t other[32];memcpy(other,conf.checkpointSha256,32U);other[0]^=1U;
    CHECK(UmiAiTrainingStackedKvStoreCheckIdentity(store,other)==UMI_STATUS_INVALID_STATE);
    float out[8]={111.0F,112.0F,113.0F,114.0F,115.0F,116.0F,117.0F,118.0F};
    CHECK(UmiAiTrainingStackedKvStoreRead(store,0U,0U,out,4U,out+1U,4U)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(out[0]==111.0F && out[1]==112.0F && out[7]==118.0F);
    CHECK(UmiAiTrainingStackedKvStoreRead(store,0U,0U,out,3U,out+4U,4U)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(out[0]==111.0F && out[7]==118.0F);
    CHECK(UmiAiTrainingStackedKvStoreReplaceRolled(store,validShift,3U,keys,24U,values,24U)==UMI_STATUS_OK);
    UmiAiTrainingStackedKvStoreDestroy(store);
    puts("multi-layer KV owner: fail-closed validation and immutable outputs passed");
    return 0;
}
