/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_generation_session_rejections.c
 * PURPOSE: Qualify refusal, output ownership, exhausted cursor, training
 *          invalidation and reversible byte constraints of stream sessions.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/generation_session.h"
#include "umicom/ai/training/byte_tokenizer.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define CHECK(expression) do { if (!(expression)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #expression); \
    return 1; } } while (0)

int main(void)
{
    UmiAiTrainingDecoderConfig config = {
        UMI_AI_BYTE_VOCABULARY, 8U, 8U, 16U, 2U, UINT64_C(20261024)
    };
    UmiAiTrainingDecoder *model = NULL, *nonByteModel = NULL;
    UmiAiTrainingGenerationSession *session = NULL;
    UmiAiTrainingNucleusOptions policy = {0.9F, 12U, 0.90F};
    const size_t prompt[] = {100U, 101U, 102U};
    size_t next = SIZE_MAX, generated = 4321U;
    uint8_t byte = 0xA5U;
    UmiAiTrainingGenerationCacheStats untouched = {999U, 888U, 777U};
    CHECK(UmiAiTrainingDecoderCreate(&config, &model) == UMI_STATUS_OK);

    CHECK(UmiAiTrainingGenerationSessionCreate(model, prompt, 3U,
        &policy, 1U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingGenerationSessionCreate(model, NULL, 1U,
        &policy, 1U, &session) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(session == NULL);
    CHECK(UmiAiTrainingGenerationSessionCreate(model, prompt,
        UMI_AI_GENERATION_MAX_PROMPT_TOKENS + 1U,
        &policy, 1U, &session) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(session == NULL);
    size_t invalidPrompt[] = {UMI_AI_BYTE_VOCABULARY};
    CHECK(UmiAiTrainingGenerationSessionCreate(model, invalidPrompt, 1U,
        &policy, 1U, &session) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(session == NULL);
    policy.topP = 0.0F;
    CHECK(UmiAiTrainingGenerationSessionCreate(model, prompt, 3U,
        &policy, 1U, &session) == UMI_STATUS_INVALID_ARGUMENT);
    policy.topP = 1.1F;
    CHECK(UmiAiTrainingGenerationSessionCreate(model, prompt, 3U,
        &policy, 1U, &session) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(session == NULL);
    policy.topP = 0.90F;
    CHECK(UmiAiTrainingGenerationSessionCreate(model, prompt, 3U,
        &policy, 1U, &session) == UMI_STATUS_OK);
    CHECK(session != NULL);
    CHECK(UmiAiTrainingGenerationSessionNext(NULL, &next) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingGenerationSessionNext(session, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingGenerationSessionInspect(NULL, &generated,
        &untouched) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingGenerationSessionInspect(session, NULL,
        &untouched) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(generated == 4321U && untouched.computedRows == 999U);
    CHECK(next == SIZE_MAX);
    CHECK(UmiAiTrainingGenerationSessionNext(session, &next) == UMI_STATUS_OK);
    CHECK(next >= UMI_AI_BYTE_OFFSET && next < UMI_AI_BYTE_VOCABULARY);
    CHECK(UmiAiTrainingGenerationSessionInspect(session, &generated,
        &untouched) == UMI_STATUS_OK && generated == 1U);
    CHECK(UmiAiTrainingGenerationSessionNextByte(session, &byte) == UMI_STATUS_OK);
    /* The returned byte may legitimately equal the sentinel (0xA5). */

    /* Training invalidates the session's borrowed decoder/cache relationship.
     * Fail closed; already emitted output remains the application's property. */
    {
        const size_t input[] = {12U, 13U, 14U, 15U, 16U, 17U, 18U, 19U};
        const size_t target[] = {13U, 14U, 15U, 16U, 17U, 18U, 19U, 20U};
        float loss = 0.0F;
        CHECK(UmiAiTrainingDecoderTrainStep(model, input, target,
            8U, 0.005F, &loss) == UMI_STATUS_OK);
        next = SIZE_MAX;
        generated = 4321U;
        untouched = (UmiAiTrainingGenerationCacheStats){999U, 888U, 777U};
        CHECK(UmiAiTrainingGenerationSessionNext(session, &next) == UMI_STATUS_INVALID_STATE);
        CHECK(next == SIZE_MAX);
        CHECK(UmiAiTrainingGenerationSessionInspect(session,
            &generated, &untouched) == UMI_STATUS_INVALID_STATE);
        CHECK(generated == 4321U && untouched.computedRows == 999U);
    }
    UmiAiTrainingGenerationSessionDestroy(session);
    session = NULL;

    /* A fresh model can run the full 256-token window. An exhausted session
     * never consumes a 257th token, even if output memory remains available. */
    CHECK(UmiAiTrainingGenerationSessionCreate(model, NULL, 0U,
        &policy, 9U, &session) == UMI_STATUS_OK);
    for (size_t i = 0U; i < UMI_AI_GENERATION_MAX_NEW_TOKENS; ++i)
        CHECK(UmiAiTrainingGenerationSessionNext(session, &next) == UMI_STATUS_OK);
    next = SIZE_MAX;
    CHECK(UmiAiTrainingGenerationSessionNext(session, &next) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(next == SIZE_MAX);
    CHECK(UmiAiTrainingGenerationSessionInspect(session,
        &generated, &untouched) == UMI_STATUS_OK);
    CHECK(generated == UMI_AI_GENERATION_MAX_NEW_TOKENS);
    UmiAiTrainingGenerationSessionDestroy(session);
    session = NULL;

    /* The byte adapter is not allowed to reinterpret a smaller-vocabulary
     * decoder as the Framework's reversible byte tokenizer. */
    config.vocabularySize = 128U;
    CHECK(UmiAiTrainingDecoderCreate(&config, &nonByteModel) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingGenerationSessionCreate(nonByteModel, NULL, 0U,
        &policy, 1U, &session) == UMI_STATUS_OK);
    byte = 0xA5U;
    CHECK(UmiAiTrainingGenerationSessionNextByte(session, &byte) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(byte == 0xA5U);
    CHECK(UmiAiTrainingGenerationSessionInspect(session,
        &generated, &untouched) == UMI_STATUS_OK && generated == 0U);
    UmiAiTrainingGenerationSessionDestroy(session);
    UmiAiTrainingDecoderDestroy(nonByteModel);
    UmiAiTrainingDecoderDestroy(model);
    UmiAiTrainingGenerationSessionDestroy(NULL);
    puts("native C23 stream invalid inputs, exhausted cursor, stale decoder: PASS");
    return 0;
}
