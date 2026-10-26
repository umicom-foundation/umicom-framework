/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_stacked_prefix_cache_plan.c
 * PURPOSE: Verify future cache workload under fixed positions and mandatory
 *          full-layer rebuild after eviction, not a claimed speed improvement.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/stacked_prefix_cache_plan.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"stacked prefix plan line %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(void)
{
    UmiAiTrainingStackedDecoderConfig config={260U,8U,8U,16U,2U,2U,37U};
    UmiAiTrainingStackedPrefixCachePlan p={0};
    CHECK(UmiAiTrainingStackedPrefixCachePlanBuild(&config,3U,12U,&p)==UMI_STATUS_OK);
    CHECK(p.canonicalTokenLayerRows==192U);
    CHECK(p.candidateTokenLayerRows==112U);
    CHECK(p.avoidedTokenLayerRows==80U);
    CHECK(p.prefixSingleRowAppends==5U && p.fullWindowRebuilds==6U);
    CHECK(p.kvTensorFp32Bytes==1024U);
    CHECK(UmiAiTrainingStackedPrefixCachePlanBuild(&config,8U,12U,&p)==UMI_STATUS_OK);
    CHECK(p.canonicalTokenLayerRows==192U && p.candidateTokenLayerRows==192U);
    CHECK(p.avoidedTokenLayerRows==0U && p.fullWindowRebuilds==11U);
    CHECK(UmiAiTrainingStackedPrefixCachePlanBuild(&config,3U,1U,&p)==UMI_STATUS_OK);
    CHECK(p.canonicalTokenLayerRows==16U && p.candidateTokenLayerRows==6U);
    UmiAiTrainingStackedPrefixCachePlan sentinel=p;
    CHECK(UmiAiTrainingStackedPrefixCachePlanBuild(&config,0U,2U,&p)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&p,&sentinel,sizeof(p))==0);
    CHECK(UmiAiTrainingStackedPrefixCachePlanBuild(&config,3U,65U,&p)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&p,&sentinel,sizeof(p))==0);
    config.attentionHeads=3U;
    CHECK(UmiAiTrainingStackedPrefixCachePlanBuild(&config,3U,2U,&p)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&p,&sentinel,sizeof(p))==0);
    puts("Bounded stacked prefix-cache planning passed (no cache execution)");
    return 0;
}
