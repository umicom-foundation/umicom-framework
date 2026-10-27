/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_stacked_incremental_step_parity.c
 *
 * PURPOSE:
 *   Compare true retained-K/V step logits AND projected K/V
 *   against two independent calls to the original stacked decoder.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "stacked_incremental_fixture.h"

static int TestLayerCount(uint32_t layers)
{
    const UmiAiTrainingStackedDecoderConfig config =
        IncrementalConfig(layers, 5U);
    UmiAiTrainingStackedDecoder *model = NULL;
    UmiAiTrainingStackedKvStore *store = NULL;
    UmiAiTrainingStackedModelIdentity identity = {0};
    UmiAiTrainingStackedKvStoreConfig storeCfg = {0};
    size_t ids[5] = {69U, 74U, 81U, 91U, 101U};
    float fullKeys[UMI_AI_STACKED_MAX_LAYERS * 5U * 8U] = {0};
    float fullValues[UMI_AI_STACKED_MAX_LAYERS * 5U * 8U] = {0};
    float canonical[5U * 260U] = {0};
    float compactKeys[UMI_AI_STACKED_MAX_LAYERS * 5U * 8U] = {0};
    float compactValues[UMI_AI_STACKED_MAX_LAYERS * 5U * 8U] = {0};
    float proposedKeys[UMI_AI_STACKED_MAX_LAYERS * 8U] = {0};
    float proposedValues[UMI_AI_STACKED_MAX_LAYERS * 8U] = {0};
    float logits[260U] = {0};
    UmiAiTrainingStackedKvStepReport report = {0};
    VERIFY(UmiAiTrainingStackedDecoderCreate(&config, &model) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedModelIdentityCapture(model, &identity) ==
           UMI_STATUS_OK);
    storeCfg.decoder = config;
    memcpy(storeCfg.checkpointSha256, identity.checkpointSha256, 32U);
    VERIFY(UmiAiTrainingStackedKvStoreCreate(&storeCfg, &store) == UMI_STATUS_OK);

    /* Canonical right padding cannot influence token zero. */
    size_t padded[5] = {ids[0], 1U, 1U, 1U, 1U};
    const size_t fullCount = (size_t)layers * 5U * 8U;
    VERIFY(UmiAiTrainingStackedDecoderTraceProjections(model,
        padded, 5U, fullKeys, fullCount, fullValues, fullCount,
        canonical, 5U * 260U) == UMI_STATUS_OK);
    for (size_t layer = 0U; layer < layers; ++layer) {
        memcpy(compactKeys + layer * 8U,
            fullKeys + layer * 5U * 8U, 8U * sizeof(float));
        memcpy(compactValues + layer * 8U,
            fullValues + layer * 5U * 8U, 8U * sizeof(float));
    }
    VERIFY(UmiAiTrainingStackedKvStorePrefill(store,
        ids, 1U, compactKeys, layers * 8U,
        compactValues, layers * 8U) == UMI_STATUS_OK);

    for (size_t retained = 1U; retained < 5U; ++retained) {
        VERIFY(UmiAiTrainingStackedKvStepPropose(model, store,
            ids[retained], proposedKeys, layers * 8U,
            proposedValues, layers * 8U, logits, 260U, &report) == UMI_STATUS_OK);
#if 0
        /* Historical placeholder-query count per layer, before native
         * single-query attention replaced multi-row inference composition. */
        VERIFY(report.retainedBefore == retained &&
               report.newlyProjectedTokenLayerRows == (uint64_t)layers &&
               report.attentionQueryLayerRowsExecuted == (uint64_t)(layers * (retained+1U)));
#endif
        VERIFY(report.retainedBefore == retained &&
               report.newlyProjectedTokenLayerRows == (uint64_t)layers &&
               report.attentionQueryLayerRowsExecuted == (uint64_t)layers);
        for (size_t t = 0U; t < 5U; ++t)
            padded[t] = t <= retained ? ids[t] : 1U;
        VERIFY(UmiAiTrainingStackedDecoderTraceProjections(model,
            padded, 5U, fullKeys, fullCount, fullValues, fullCount,
            canonical, 5U * 260U) == UMI_STATUS_OK);
        for (size_t token = 0U; token < 260U; ++token)
            VERIFY(IncrementalNear(logits[token],
                canonical[retained * 260U + token]));
        for (size_t layer = 0U; layer < layers; ++layer)
            for (size_t c = 0U; c < 8U; ++c) {
                const size_t offset = (layer * 5U + retained) * 8U + c;
                VERIFY(IncrementalNear(proposedKeys[layer * 8U + c],
                                        fullKeys[offset]));
                VERIFY(IncrementalNear(proposedValues[layer * 8U + c],
                                        fullValues[offset]));
            }
        VERIFY(UmiAiTrainingStackedKvStoreAppend(store, ids[retained],
            proposedKeys, layers * 8U,
            proposedValues, layers * 8U) == UMI_STATUS_OK);
    }
    UmiAiTrainingStackedKvStoreDestroy(store);
    UmiAiTrainingStackedDecoderDestroy(model);
    return 0;
}

int main(void)
{
    VERIFY(TestLayerCount(1U) == 0);
    VERIFY(TestLayerCount(2U) == 0);
    VERIFY(TestLayerCount(3U) == 0);
    puts("Stacked KV step: every vocabulary logit and actual K/V row agrees");
    return 0;
}
