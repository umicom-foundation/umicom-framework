/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_stacked_prefix_causality.c
 *
 * PURPOSE:
 *   Compare all unchanged-prefix vocabulary logits under distinct FUTURE suffix tokens on the actual multi-layer Transformer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/stacked_prefix_qualification.h"
#include <math.h>
#include <stdio.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "causality line %d: %s\n", __LINE__, #x); return 1; } } while (0)
int main(void)
{
    const UmiAiTrainingStackedDecoderConfig cfg = {32U, 4U, 4U, 8U, 2U, 3U, UINT64_C(4511)};
    const size_t prefix[] = {6U, 10U};
    UmiAiTrainingStackedDecoder *model = NULL;
    UmiAiTrainingStackedPrefixQualification evidence = {0};
    CHECK(UmiAiTrainingStackedDecoderCreate(&cfg, &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedPrefixCausalityQualify(model, prefix, 2U,
          &evidence) == UMI_STATUS_OK);
    CHECK(evidence.comparedLogits == 64U);
    CHECK(evidence.unchangedPrefixTokens == 2U && evidence.transformerLayers == 3U);
    CHECK(isfinite(evidence.maximumAbsoluteDifference));
    CHECK(evidence.maximumAbsoluteDifference <= 0.00004F);
    UmiAiTrainingStackedDecoderDestroy(model);
    puts("stacked prefix: causal suffix independence passed on real decoder");
    return 0;
}
