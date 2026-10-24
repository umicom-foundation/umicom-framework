/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_position_contract_benchmark.c
 * PURPOSE: Gate the qualified canonical/prefix benchmark with an explicit,
 *          checkpoint-bound context declaration without changing its paths.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/position_contract.h"
#include "umicom/ai/training/prefix_benchmark.h"
#include "umicom/ai/training/byte_tokenizer.h"

#include <stdint.h>
#include <stdio.h>

#define CHECK(expression) do { if (!(expression)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #expression); return 1; \
} } while (0)

int main(void)
{
    const UmiAiTrainingDecoderConfig config = {
        UMI_AI_BYTE_VOCABULARY, 8U, 8U, 16U, 2U, UINT64_C(20261024)
    };
    const size_t prompt[] = {109U, 114U, 120U}; /* "int" byte IDs */
    const UmiAiTrainingNucleusOptions sampling = {1.0F, 1U, 1.0F};
    const UmiAiTrainingPrefixBenchmarkPlan plan = {
        6U, 0U, 1U, UINT64_C(7832)
    };
    UmiAiTrainingDecoder *model = NULL;
    UmiAiTrainingPositionContract prefix = {0};
    UmiAiTrainingPrefixBenchmarkResult benchmark = {0};
    CHECK(UmiAiTrainingDecoderCreate(&config, &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingPositionContractBuild(model,
        UMI_AI_POSITION_CONTEXT_GROWING_UNPADDED, &prefix) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingPositionContractVerify(model, &prefix,
        UMI_AI_POSITION_CONTEXT_GROWING_UNPADDED) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingPrefixBenchmarkRun(model, prompt, 3U, &sampling,
        &plan, &benchmark) == UMI_STATUS_OK);
    CHECK(benchmark.checkpointUnchanged == 1);
    CHECK(benchmark.verifiedPairs == 1U);
    CHECK(benchmark.comparedTokens == plan.generatedTokens);
    CHECK(benchmark.estimate.cachedComputedRows ==
          benchmark.observedCache.computedRows);
    CHECK(UmiAiTrainingPositionContractVerify(model, &prefix,
        UMI_AI_POSITION_CONTEXT_GROWING_UNPADDED) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingPositionContractVerify(model, &prefix,
        UMI_AI_POSITION_CONTEXT_BOS_LEFT_PADDED) == UMI_STATUS_INVALID_STATE);
    UmiAiTrainingDecoderDestroy(model);
    puts("position contract: checkpoint-bound native benchmark admission PASS");
    return 0;
}
