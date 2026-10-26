/* Umicom Framework | Session ownership regression | Sammy Hegab | MIT */
#include "stacked_inference_fixture.h"
int main(void)
{
    UmiAiTrainingStackedDecoder *model = NULL;
    UmiAiTrainingStackedGenerationSession *session = NULL;
    UmiAiTrainingStackedGenerationStats stats = {0};
    const UmiAiTrainingNucleusOptions policy = {1.0F, 1U, 1.0F};
    const size_t prompt[] = {109U, 114U, 120U};
    size_t generated[7] = {0};
    size_t written = 0U;
    CHECK(MakeModel(UINT64_C(41), &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedGenerationSessionCreate(model, prompt, 3U,
        &policy, UINT64_C(45), &session) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedGenerationSessionInspect(session, &stats) == UMI_STATUS_OK);
    CHECK(stats.generatedTokens == 0U && stats.computedFullContextTokenRows == 0U);
    for (size_t i = 0U; i < 7U; ++i) {
        CHECK(UmiAiTrainingStackedGenerationSessionNext(session, &generated[i]) == UMI_STATUS_OK);
        CHECK(generated[i] >= 4U && generated[i] < 260U);
    }
    CHECK(UmiAiTrainingStackedGenerationSessionInspect(session, &stats) == UMI_STATUS_OK);
    CHECK(stats.generatedTokens == 7U && stats.contextTokens == 4U);
    CHECK(stats.computedFullContextTokenRows == 28U);
    CHECK(stats.computedTransformerTokenLayerRows == 56U);
    size_t oneShot[7] = {0};
    CHECK(UmiAiTrainingStackedGenerateNucleus(model, prompt, 3U,
        &policy, 7U, UINT64_C(45), oneShot, 7U, &written, NULL) == UMI_STATUS_OK);
    CHECK(written == 7U && memcmp(oneShot, generated, sizeof(oneShot)) == 0);
    UmiAiTrainingStackedGenerationSessionDestroy(session);
    UmiAiTrainingStackedDecoderDestroy(model);
    puts("stacked session: streaming and one-shot output agree");
    return 0;
}
