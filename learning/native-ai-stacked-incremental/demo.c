/*-----------------------------------------------------------------------------
 * Umicom Framework | Native incremental-stack teaching demonstration
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/stacked_kv_incremental.h"
#include "umicom/ai/training/stacked_prefix_inference.h"
#include <inttypes.h>
#include <stdio.h>

int main(void)
{
    const UmiAiTrainingStackedDecoderConfig config = {
        260U, 6U, 8U, 12U, 2U, 2U, UINT64_C(20261026)
    };
    const UmiAiTrainingNucleusOptions policy = {1.0F, 1U, 1.0F};
    const size_t prompt[] = {76U, 80U};
    UmiAiTrainingStackedDecoder *model = NULL;
    UmiAiTrainingStackedKvIncrementalStats stats = {0};
    size_t actual[10] = {0}, reference[10] = {0};
    size_t used = 0U, refCount = 0U;
    UmiStatus status = UmiAiTrainingStackedDecoderCreate(&config, &model);
    if (status != UMI_STATUS_OK) return 1;
    status = UmiAiTrainingStackedKvIncrementalGenerate(model,
        prompt, 2U, &policy, 10U, UINT64_C(17),
        actual, 10U, &used, &stats);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingStackedPrefixGenerateNucleus(model,
            prompt, 2U, &policy, 10U, UINT64_C(17),
            reference, 10U, &refCount, NULL);
    if (status != UMI_STATUS_OK || used != refCount) {
        UmiAiTrainingStackedDecoderDestroy(model);
        return 2;
    }
    for (size_t index = 0U; index < used; ++index) {
        if (actual[index] != reference[index]) {
            UmiAiTrainingStackedDecoderDestroy(model);
            return 3;
        }
    }
    printf("Canonical prefix token parity: %zu of %zu\n", used, refCount);
    printf("Reference full-context token-layer rows: %" PRIu64 "\n",
        stats.referenceFullContextTokenLayerRows);
    printf("Observed full-context token-layer rows: %" PRIu64 "\n",
        stats.observedFullContextTokenLayerRows);
    printf("Observed incrementally projected token-layer rows: %" PRIu64 "\n",
        stats.observedIncrementalProjectionTokenLayerRows);
    printf("Observed attention query-layer rows: %" PRIu64 "\n",
        stats.observedAttentionQueryTokenLayerRows);
    printf("Full-window rebuilds: %" PRIu64 "\n",
        stats.observedFullWindowRebuilds);
    puts("Token parity and row counts are NOT a measured speedup.");
    UmiAiTrainingStackedDecoderDestroy(model);
    return 0;
}
