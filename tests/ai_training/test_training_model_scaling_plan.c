/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_model_scaling_plan.c
 *
 * PURPOSE:
 *   Check model scaling estimates, shape validation and arithmetic overflow.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/model_scaling_plan.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#define CHECK(x) do {if (!(x)) { fprintf(stderr,"failed line %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(void) {
    UmiAiTrainingModelScalingShape s = {260U,8U,8U,16U,2U,1U,1U,2U};
    UmiAiTrainingModelScalingEstimate e = {0};
    CHECK(UmiAiTrainingModelScalingEstimateBuild(&s,&e)==UMI_STATUS_OK);
    CHECK(e.tokenEmbeddingParameters==2080U);
    CHECK(e.positionEmbeddingParameters==64U);
    CHECK(e.transformerParameters== (4U*64U + 2U*128U + 4U*8U + 16U + 8U));
    CHECK(e.logitsParameters==2080U);
    CHECK(e.totalParameters==2080U+64U+e.transformerParameters+2080U);
    CHECK(e.adapterParameters==2U*(8U*8U+2U*(8U+16U)));
    CHECK(e.kvCacheFp32Bytes==8U*8U*2U*4U);
    CHECK(e.adapterAdamwMomentBytes==e.adapterParameters*16U);
    CHECK(e.minimumEstimateBytes==e.frozenFp32Bytes+e.adapterFp32Bytes+
       e.adapterSgdGradientBytes+e.adapterAdamwMomentBytes+e.kvCacheFp32Bytes);
    UmiAiTrainingModelScalingEstimate before=e;
    s.attentionHeads=3U;
    CHECK(UmiAiTrainingModelScalingEstimateBuild(&s,&e)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&e,&before,sizeof(e))==0);
    s.attentionHeads=2U;
    s.adapterRank=17U;
    CHECK(UmiAiTrainingModelScalingEstimateBuild(&s,&e)==UMI_STATUS_INVALID_ARGUMENT);
    s.adapterRank=0U; s.transformerLayers=UINT64_MAX;
    CHECK(UmiAiTrainingModelScalingEstimateBuild(&s,&e)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(&e,&before,sizeof(e))==0);
    CHECK(UmiAiTrainingModelScalingEstimateBuild(NULL,&e)==UMI_STATUS_INVALID_ARGUMENT);
    puts("model scaling shapes, FP32 estimates, and overflow rejection passed");
    return 0;
}
