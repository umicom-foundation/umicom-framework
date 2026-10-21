/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_capacity_plan.c
 * PURPOSE: Reconcile planned parameter sizes with canonical decoder shapes;
 *          refuse impossible tensors without touching caller output.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/capacity_plan.h"
#include "umicom/ai/training/tape.h"
#include <stdio.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #x); return 1; \
} } while (0)

int main(void)
{
    UmiAiTrainingDecoderConfig cfg = {260U, 8U, 8U, 16U, 2U, 20261021U};
    UmiAiTrainingCapacityPlan plan = {0};
    UmiAiTrainingCapacityPlan preserved;

    CHECK(UmiAiTrainingCapacityPlanCompute(&cfg, 1U, &plan) == UMI_STATUS_OK);
    CHECK(plan.parameterTensors == 15U && plan.parameterElements == 4792U);
    CHECK(plan.parameterValueBytes == 4792U * sizeof(float));
    CHECK(plan.parameterValueBytes == plan.parameterGradientBytes);
    CHECK(plan.tokensPerBatch == 8U && plan.logitsElements == 2080U);
    CHECK(plan.attentionProbabilityElements == 128U);
    CHECK(plan.maximumBatchSequences == 1U);
    CHECK(plan.minimumRetainedPayloadBytes ==
          plan.parameterValueBytes * 2U + plan.logitsBytes +
          plan.attentionProbabilityBytes);
    CHECK(plan.maximumTensorElements == UMI_AI_TRAINING_MAX_ELEMENTS);

    preserved = plan;
    CHECK(UmiAiTrainingCapacityPlanCompute(&cfg, 2U, &plan)
          == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(&plan, &preserved, sizeof(plan)) == 0);
    cfg.channels = 32U;
    CHECK(UmiAiTrainingCapacityPlanCompute(&cfg, 1U, &plan)
          == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(&plan, &preserved, sizeof(plan)) == 0);
    cfg.channels = 8U;
    cfg.attentionHeads = 3U;
    CHECK(UmiAiTrainingCapacityPlanCompute(&cfg, 1U, &plan)
          == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&plan, &preserved, sizeof(plan)) == 0);

    cfg = (UmiAiTrainingDecoderConfig){8U, 4U, 4U, 8U, 2U, 1U};
    CHECK(UmiAiTrainingCapacityPlanCompute(&cfg, 128U, &plan) == UMI_STATUS_OK);
    CHECK(plan.maximumBatchSequences == 128U);
    preserved = plan;
    CHECK(UmiAiTrainingCapacityPlanCompute(&cfg, 129U, &plan)
          == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(&plan, &preserved, sizeof(plan)) == 0);
    CHECK(UmiAiTrainingCapacityPlanCompute(&cfg, 0U, &plan)
          == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&plan, &preserved, sizeof(plan)) == 0);
    puts("Decoder capacity planning and failure-atomic shapes: PASS");
    return 0;
}
