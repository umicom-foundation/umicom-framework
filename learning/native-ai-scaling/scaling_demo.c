/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: learning/native-ai-scaling/scaling_demo.c
 *
 * PURPOSE:
 *   Demonstrate model parameter and memory estimates without allocating a model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/model_scaling_plan.h"
#include <inttypes.h>
#include <stdio.h>
int main(void) {
    UmiAiTrainingModelScalingShape shape={260U,8U,8U,16U,2U,4U,2U,2U};
    UmiAiTrainingModelScalingEstimate estimate={0};
    if(UmiAiTrainingModelScalingEstimateBuild(&shape,&estimate)!=UMI_STATUS_OK) return 1;
    printf("total parameters: %" PRIu64 "\n",estimate.totalParameters);
    printf("trainable adapter parameters: %" PRIu64 "\n",estimate.adapterParameters);
    printf("KV cache FP32 bytes: %" PRIu64 "\n",estimate.kvCacheFp32Bytes);
    printf("partial capacity estimate bytes: %" PRIu64 "\n",estimate.minimumEstimateBytes);
    puts("This is a projection, not a model allocation or a training run.");
    return 0;
}
