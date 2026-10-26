/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: learning/native-ai-stacked-inference/demo.c
 * PURPOSE: Demonstrate reproducible sampling and aggregate held-out metrics.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/stacked_generation_evidence.h"
#include "umicom/ai/training/stacked_evaluation.h"
#include "umicom/ai/training/byte_tokenizer.h"

#include <inttypes.h>
#include <stdio.h>

int main(void)
{
    const UmiAiTrainingStackedDecoderConfig config = {
        UMI_AI_BYTE_VOCABULARY, 4U, 8U, 16U, 2U, 2U, UINT64_C(20261026)
    };
    const UmiAiTrainingNucleusOptions policy = {1.0F, 1U, 1.0F};
    const size_t prompt[] = {UMI_AI_BYTE_OFFSET + (size_t)'i',
                             UMI_AI_BYTE_OFFSET + (size_t)'n'};
    const size_t inputs[4] = {1U, prompt[0], prompt[1], 2U};
    const size_t targets[4] = {0U, prompt[1], 42U, 43U};
    const uint8_t mask[4] = {0U, 1U, 1U, 0U};
    UmiAiTrainingStackedEvaluationCase heldout = {
        .inputIds = inputs, .targetIds = targets, .lossMask = mask,
        .tokenCount = 4U
    };
    UmiAiTrainingStackedDecoder *model = NULL;
    UmiAiTrainingStackedGenerationStats stats = {0};
    UmiAiTrainingStackedGenerationEvidence evidence = {0};
    UmiAiTrainingStackedEvaluationReport evaluation = {0};
    size_t samples[6] = {0}, count = 0U;
    UmiStatus status = UmiAiTrainingStackedDecoderCreate(&config, &model);
    if (status != UMI_STATUS_OK) return 1;
    status = UmiAiTrainingStackedGenerateNucleus(model, prompt, 2U,
        &policy, 6U, UINT64_C(47), samples, 6U, &count, &stats);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingStackedGenerationEvidenceBuild(model, prompt,
            2U, &policy, UINT64_C(47), samples, count, &evidence);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingStackedEvaluationRun(model, &heldout, 1U,
                                                   &evaluation);
    if (status == UMI_STATUS_OK) {
        printf("Generated tokens: %zu\n", count);
        printf("Full-context rows: %" PRIu64 "\n",
               stats.computedFullContextTokenRows);
        printf("Transformer layer-token rows: %" PRIu64 "\n",
               stats.computedTransformerTokenLayerRows);
        printf("Held-out selected loss: %.6f\n",
               evaluation.meanSelectedCrossEntropy);
        printf("Held-out top-one accuracy: %.4f\n",
               evaluation.topOneAccuracy);
        printf("Reproducibility record tokens: %zu\n",
               evidence.outputTokenCount);
    }
    UmiAiTrainingStackedDecoderDestroy(model);
    return status == UMI_STATUS_OK ? 0 : 2;
}
