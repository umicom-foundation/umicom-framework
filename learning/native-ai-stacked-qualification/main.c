/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: learning/native-ai-stacked-qualification/main.c
 *
 * PURPOSE:
 *   Demonstrate all-logit, per-layer K/V and rollover qualification of an
 *   actual bounded stacked decoder without implying speed improvements.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/stacked_inference_qualification.h"
#include "umicom/ai/training/stacked_decoder.h"

#include <stdio.h>
#include <stdint.h>

int main(void)
{
    const UmiAiTrainingStackedDecoderConfig config = {
        260U, 6U, 8U, 12U, 2U, 2U, UINT64_C(74112)
    };
    const size_t prefix[] = {48U, 53U};
    const size_t continuation[] = {54U, 56U, 58U, 60U, 62U, 64U, 66U, 68U};
    const UmiAiTrainingInferenceParityTolerance tolerance = {
        0.0001F, 0.0001F
    };
    UmiAiTrainingStackedDecoder *model = NULL;
    UmiAiTrainingStackedQualificationReport evidence = {0};
    UmiStatus status = UmiAiTrainingStackedDecoderCreate(&config, &model);
    if (status != UMI_STATUS_OK) {
        fputs("Unable to create training model\n", stderr);
        return 1;
    }
    status = UmiAiTrainingStackedInferenceQualify(model,
        prefix, 2U, continuation, 8U, &tolerance, &evidence);
    if (status == UMI_STATUS_OK) {
        printf("qualified: %zu single-query steps, %zu full rebuilds\n",
            evidence.growingPrefixSteps, evidence.fullWindowRebuilds);
        printf("logits: %zu canonical, %zu candidate, %zu causal prefix\n",
            evidence.independentCanonicalLogitsCompared,
            evidence.incrementalLogitsCompared,
            evidence.unchangedPrefixLogitsCompared);
        printf("K/V elements: %zu proposed, %zu cached\n",
            evidence.incrementalKeyValueScalarsCompared,
            evidence.retainedKeyValueScalarsCompared);
        puts("No performance result was measured by this program.");
    } else {
        fprintf(stderr, "qualification failed: status=%d\n", (int)status);
    }
    UmiAiTrainingStackedDecoderDestroy(model);
    return status == UMI_STATUS_OK ? 0 : 1;
}
