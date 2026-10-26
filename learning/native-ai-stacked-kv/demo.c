/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: learning/native-ai-stacked-kv/demo.c
 * PURPOSE: Explain actual native K/V row ownership and the full rollover
 *          rebuild requirement without misrepresenting cache throughput.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation | LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/stacked_kv_work.h"
#include <inttypes.h>
#include <stdio.h>
int main(void)
{
    const UmiAiTrainingStackedDecoderConfig shape =
        {260U,4U,4U,8U,2U,3U,UINT64_C(7)};
    UmiAiTrainingStackedKvStoreConfig config = {.decoder=shape};
    UmiAiTrainingStackedKvStore *store=NULL;
    UmiAiTrainingStackedPrefixCachePlan plan={0};
    UmiAiTrainingStackedKvWorkAudit audit={0};
    float keys[48]={0},values[48]={0};
    size_t ids[4]={10U,11U,12U,13U};
    for(size_t i=0U;i<32U;++i)config.checkpointSha256[i]=(uint8_t)(i+1U);
    for(size_t i=0U;i<48U;++i){keys[i]=(float)i*0.015F;values[i]=(float)i*0.017F;}
    if(UmiAiTrainingStackedPrefixCachePlanBuild(&shape,2U,7U,&plan)!=UMI_STATUS_OK)
        return 1;
    if(UmiAiTrainingStackedKvStoreCreate(&config,&store)!=UMI_STATUS_OK)
        return 2;
    UmiStatus status=UmiAiTrainingStackedKvStorePrefill(store,ids,2U,
                                                        keys,24U,values,24U);
    if(status==UMI_STATUS_OK)status=UmiAiTrainingStackedKvStoreAppend(store,12U,
                                                                       keys,12U,values,12U);
    if(status==UMI_STATUS_OK)status=UmiAiTrainingStackedKvStoreAppend(store,13U,
                                                                       keys,12U,values,12U);
    for(size_t i=0U;i<4U && status==UMI_STATUS_OK;++i){
        ids[0]=ids[1];ids[1]=ids[2];ids[2]=ids[3];ids[3]=14U+i;
        status=UmiAiTrainingStackedKvStoreReplaceRolled(store,ids,4U,
                                                          keys,48U,values,48U);
    }
    if(status==UMI_STATUS_OK)status=UmiAiTrainingStackedKvWorkQualify(store,&plan,&audit);
    if(status==UMI_STATUS_OK){
        printf("Actual stored K/V token-layer rows: %" PRIu64 "\n",audit.observedAcceptedTokenLayerRows);
        printf("Full reference work forecast: %" PRIu64 "\n",plan.canonicalTokenLayerRows);
        printf("Conditional rows avoided: %" PRIu64 "\n",audit.conditionalRowsAvoided);
        printf("Full-window replacements: %" PRIu64 "\n",audit.observedRebuilds);
        puts("Synthetic supplied K/V rows; NOT canonical decoder parity or a speed benchmark.");
    }
    UmiAiTrainingStackedKvStoreDestroy(store);
    return status==UMI_STATUS_OK?0:3;
}
