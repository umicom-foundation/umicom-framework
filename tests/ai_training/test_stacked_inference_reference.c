/* Umicom Framework | Independent canonical decoder replay | Sammy Hegab | MIT */
#include "stacked_inference_fixture.h"
#include <stdlib.h>

/* The test's separate full-context replay is not shipped in the production
 * inference service. It exercises the existing canonical logits and sampler. */
static int CheckReplay(UmiAiTrainingStackedDecoder *model,
                       const UmiAiTrainingNucleusOptions *policy)
{
    size_t context[4] = {1U, 1U, 109U, 114U};
    const size_t initial[] = {109U, 114U};
    float logits[4U * 260U] = {0};
    size_t replayed[6] = {0}, result[6] = {0}, written = 0U;
    UmiAiTrainingRandomState random = {0};
    CHECK(UmiAiTrainingRandomSeed(&random, UINT64_C(73)) == UMI_STATUS_OK);
    for (size_t i = 0U; i < 6U; ++i) {
        CHECK(UmiAiTrainingStackedDecoderLogits(model, context, 4U,
                                                logits, 4U * 260U) == UMI_STATUS_OK);
        CHECK(UmiAiTrainingSampleLogitsNucleus(
            logits + 3U * 260U, 260U, 4U, 260U, policy,
            &random, &replayed[i], NULL) == UMI_STATUS_OK);
        memmove(context, context + 1U, 3U * sizeof(*context));
        context[3] = replayed[i];
    }
    CHECK(UmiAiTrainingStackedGenerateNucleus(model, initial, 2U,
          policy, 6U, UINT64_C(73), result, 6U, &written, NULL) == UMI_STATUS_OK);
    CHECK(written == 6U && memcmp(replayed, result, sizeof(result)) == 0);
    return 0;
}
int main(void)
{
    UmiAiTrainingStackedDecoder *model = NULL;
    const UmiAiTrainingNucleusOptions greedy = {1.0F, 1U, 1.0F};
    const UmiAiTrainingNucleusOptions nucleus = {0.9F, 9U, 0.92F};
    CHECK(MakeModel(UINT64_C(11), &model) == UMI_STATUS_OK);
    CHECK(CheckReplay(model, &greedy) == 0);
    CHECK(CheckReplay(model, &nucleus) == 0);
    UmiAiTrainingStackedDecoderDestroy(model);
    puts("stacked inference: independent full-context sampling replay passed");
    return 0;
}
