/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: learning/native-ai-stacked-prefix/demo.c
 *
 * PURPOSE:
 *   Show real full-context work, causal-prefix evidence and the separate CONDITIONAL cache work estimate without claiming speedup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/stacked_prefix_inference.h"
#include "umicom/ai/training/stacked_prefix_qualification.h"
#include "umicom/ai/training/stacked_prefix_cache_plan.h"
#include <inttypes.h>
#include <stdio.h>
int main(void)
{
    const UmiAiTrainingStackedDecoderConfig config = {32U, 8U, 8U, 12U, 2U, 2U, UINT64_C(20261026)};
    const UmiAiTrainingNucleusOptions policy = {1.0F, 1U, 1.0F};
    const size_t prompt[3] = {6U, 7U, 8U};
    UmiAiTrainingStackedDecoder *model = NULL;
    UmiAiTrainingStackedPrefixStats observed = {0};
    UmiAiTrainingStackedPrefixCachePlan planned = {0};
    UmiAiTrainingStackedPrefixQualification qualified = {0};
    size_t output[12] = {0}, count = 0U;
    UmiStatus status = UmiAiTrainingStackedDecoderCreate(&config, &model);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingStackedPrefixCausalityQualify(
            model, prompt, 3U, &qualified);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingStackedPrefixCachePlanBuild(&config, 3U, 12U, &planned);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingStackedPrefixGenerateNucleus(
            model, prompt, 3U, &policy, 12U, UINT64_C(7801),
            output, 12U, &count, &observed);
    if (status == UMI_STATUS_OK) {
        printf("Causal prefix logits checked: %zu\n", qualified.comparedLogits);
        printf("Generated token IDs: %zu\n", count);
        printf("Observed full-context token-layer rows: %" PRIu64 "\n",
               observed.computedTransformerTokenLayerRows);
        printf("Hypothetical cache token-layer rows: %" PRIu64 "\n",
               planned.candidateTokenLayerRows);
        puts("The hypothetical work is NOT measured cache work. No KV cache was used.");
    }
    UmiAiTrainingStackedDecoderDestroy(model);
    return status == UMI_STATUS_OK ? 0 : 1;
}
