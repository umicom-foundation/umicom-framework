/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_stacked_kv_work.c
 *
 * PURPOSE:
 *   Check real cache-storage row counters against the established Framework conditional work planner for prefix and rollover.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/stacked_kv_work.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do {if (!(x)) {fprintf(stderr, "kv work %d: %s\n", __LINE__, #x); return 1;}} while(0)
int main(void)
{
    const UmiAiTrainingStackedDecoderConfig model = {
        260U,4U,4U,8U,2U,3U,UINT64_C(73)};
    UmiAiTrainingStackedKvStoreConfig config = {.decoder=model};
    UmiAiTrainingStackedKvStore *store=NULL;
    UmiAiTrainingStackedPrefixCachePlan plan={0},wrong={0};
    UmiAiTrainingStackedKvWorkAudit actual={0};
    size_t ids[4]={10U,11U,12U,13U};
    float keys[48]={0},values[48]={0};
    for(size_t i=0U;i<32U;++i)config.checkpointSha256[i]=(uint8_t)(i+1U);
    for(size_t i=0U;i<48U;++i){keys[i]=(float)i*0.01F;values[i]=(float)i*0.02F;}
    CHECK(UmiAiTrainingStackedPrefixCachePlanBuild(&model,2U,7U,&plan)==UMI_STATUS_OK);
    CHECK(plan.canonicalTokenLayerRows==84U);
    CHECK(plan.candidateTokenLayerRows==60U);
    CHECK(plan.avoidedTokenLayerRows==24U);
    CHECK(UmiAiTrainingStackedKvStoreCreate(&config,&store)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedKvStorePrefill(store,ids,2U,keys,24U,values,24U)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedKvStoreAppend(store,12U,keys,12U,values,12U)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedKvStoreAppend(store,13U,keys,12U,values,12U)==UMI_STATUS_OK);
    for(size_t step=0U;step<4U;++step){
        ids[0]=ids[1];ids[1]=ids[2];ids[2]=ids[3];ids[3]=14U+step;
        CHECK(UmiAiTrainingStackedKvStoreReplaceRolled(store,ids,4U,
              keys,48U,values,48U)==UMI_STATUS_OK);
    }
    CHECK(UmiAiTrainingStackedKvWorkQualify(store,&plan,&actual)==UMI_STATUS_OK);
    CHECK(actual.observedAcceptedTokenLayerRows==60U && actual.plannedTokenLayerRows==60U);
    CHECK(actual.conditionalRowsAvoided==24U && actual.observedRebuilds==4U);
    CHECK(UmiAiTrainingStackedPrefixCachePlanBuild(&model,2U,6U,&wrong)==UMI_STATUS_OK);
    actual.plannedTokenLayerRows=123456U;
    CHECK(UmiAiTrainingStackedKvWorkQualify(store,&wrong,&actual)==UMI_STATUS_INVALID_STATE);
    CHECK(actual.plannedTokenLayerRows==123456U);
    UmiAiTrainingStackedKvStoreDestroy(store);
    puts("multi-layer KV storage work matches canonical conditional planner");
    return 0;
}
