/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_stacked_inference_rejections.c
 *
 * PURPOSE:
 *   Rejection and failure atomicity
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "stacked_inference_fixture.h"
int main(void)
{
    UmiAiTrainingStackedDecoder *model = NULL;
    UmiAiTrainingStackedGenerationSession *session = NULL;
    const UmiAiTrainingNucleusOptions policy = {1.0F, 1U, 1.0F};
    const UmiAiTrainingNucleusOptions invalidPolicy = {1.0F, 1U, 1.5F};
    const size_t invalidLongPrompt[] = {999U, 109U, 114U, 120U, 121U};
    const size_t goodPrompt[] = {109U};
    size_t tokens[3] = {777U, 777U, 777U}, written = 888U;
    UmiAiTrainingStackedGenerationStats stats;
    memset(&stats, 0x66, sizeof(stats));
    UmiAiTrainingStackedGenerationStats untouched = stats;
    CHECK(MakeModel(UINT64_C(11), &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedGenerationSessionCreate(model,
        invalidLongPrompt, 5U, &policy, 8U, &session) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(session == NULL);
    CHECK(UmiAiTrainingStackedGenerationSessionCreate(model,
        goodPrompt, 1U, &invalidPolicy, 8U, &session) != UMI_STATUS_OK);
    CHECK(session == NULL);
    CHECK(UmiAiTrainingStackedGenerateNucleus(model, goodPrompt, 1U,
        &policy, 3U, 8U, tokens, 2U, &written, &stats) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(tokens[0] == 777U && written == 888U &&
          memcmp(&stats, &untouched, sizeof(stats)) == 0);
    CHECK(UmiAiTrainingStackedGenerateNucleus(model, goodPrompt, 1U,
        &policy, 0U, 8U, tokens, 3U, &written, &stats) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingStackedGenerateNucleus(model, goodPrompt, 1U,
        &policy, 65U, 8U, tokens, 3U, &written, &stats) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(written == 888U && memcmp(&stats, &untouched, sizeof(stats)) == 0);
    CHECK(UmiAiTrainingStackedGenerationSessionCreate(model, goodPrompt, 1U,
        &policy, 8U, &session) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedGenerationSessionNext(session, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingStackedGenerationSessionInspect(session, &stats) == UMI_STATUS_OK);
    CHECK(stats.generatedTokens == 0U);
    UmiAiTrainingStackedGenerationSessionDestroy(session);
    UmiAiTrainingStackedDecoderDestroy(model);
    puts("stacked inference: public rejections preserve outputs and cursor");
    return 0;
}
