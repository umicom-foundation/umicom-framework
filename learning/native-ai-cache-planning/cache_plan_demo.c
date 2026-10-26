/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: learning/native-ai-cache-planning/cache_plan_demo.c
 * PURPOSE: Demonstrate conditional layer-token work and safe eviction costs.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/stacked_prefix_cache_plan.h"
#include <inttypes.h>
#include <stdio.h>
int main(void)
{
    UmiAiTrainingStackedDecoderConfig model={260U,8U,8U,16U,2U,2U,37U};
    UmiAiTrainingStackedPrefixCachePlan plan={0};
    UmiStatus s=UmiAiTrainingStackedPrefixCachePlanBuild(&model,3U,12U,&plan);
    if(s!=UMI_STATUS_OK){fprintf(stderr,"Cache preflight failed (%d)\n",(int)s);return 1;}
    printf("Canonical token-layer rows: %" PRIu64 "\n",plan.canonicalTokenLayerRows);
    printf("Hypothetical cache token-layer rows: %" PRIu64 "\n",plan.candidateTokenLayerRows);
    printf("Full-window rebuilds required: %" PRIu64 "\n",plan.fullWindowRebuilds);
    printf("Raw KV array estimate: %" PRIu64 " bytes\n",plan.kvTensorFp32Bytes);
    puts("Planning only: no cache, numerical parity or latency acceleration is established.");
    return 0;
}
