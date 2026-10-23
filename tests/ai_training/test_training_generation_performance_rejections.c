/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_generation_performance_rejections.c
 * PURPOSE: Reject invalid workload proposals without corrupting report output.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/generation_performance.h"
#include "umicom/ai/training/byte_tokenizer.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #expr); return 1; \
} } while (0)

int main(void)
{
    UmiAiTrainingDecoderConfig config = {
        UMI_AI_BYTE_VOCABULARY, 8U, 8U, 16U, 2U, UINT64_C(66)
    };
    UmiAiTrainingNucleusOptions policy = {0.85F, 12U, 0.9F};
    UmiAiTrainingDecoder *model = NULL;
    UmiAiTrainingGenerationPerformance report;
    UmiAiTrainingGenerationWorkEstimate estimate;
    const size_t prompt[] = {4U, 5U, 6U};
    const size_t badPrompt[] = {UMI_AI_BYTE_VOCABULARY};
    unsigned char savedEstimate[sizeof estimate];
    unsigned char savedReport[sizeof report];

    memset(&estimate, 0xAC, sizeof estimate);
    memcpy(savedEstimate, &estimate, sizeof estimate);
    CHECK(UmiAiTrainingGenerationWorkEstimateBuild(NULL, 0U, 1U,
        &estimate) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingGenerationWorkEstimateBuild(&config, 0U, 0U,
        &estimate) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingGenerationWorkEstimateBuild(&config,
        UMI_AI_GENERATION_MAX_PROMPT_TOKENS + 1U, 1U,
        &estimate) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingGenerationWorkEstimateBuild(&config, 0U,
        UMI_AI_GENERATION_MAX_NEW_TOKENS + 1U,
        &estimate) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&estimate, savedEstimate, sizeof estimate) == 0);
    config.attentionHeads = 3U;
    CHECK(UmiAiTrainingGenerationWorkEstimateBuild(&config, 0U, 1U,
        &estimate) != UMI_STATUS_OK);
    CHECK(memcmp(&estimate, savedEstimate, sizeof estimate) == 0);
    config.attentionHeads = 2U;

    CHECK(UmiAiTrainingDecoderCreate(&config, &model) == UMI_STATUS_OK);
    memset(&report, 0xAB, sizeof report);
    memcpy(savedReport, &report, sizeof report);
    CHECK(UmiAiTrainingGenerationPerformanceCompare(NULL, prompt, 3U,
        &policy, 3U, 99U, &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingGenerationPerformanceCompare(model, NULL, 3U,
        &policy, 3U, 99U, &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingGenerationPerformanceCompare(model, prompt, 3U,
        &policy, 0U, 99U, &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingGenerationPerformanceCompare(model, badPrompt, 1U,
        &policy, 2U, 99U, &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingGenerationPerformanceCompare(model, prompt, 3U,
        NULL, 2U, 99U, &report) == UMI_STATUS_INVALID_ARGUMENT);
    policy.topP = 0.0F;
    CHECK(UmiAiTrainingGenerationPerformanceCompare(model, prompt, 3U,
        &policy, 2U, 99U, &report) == UMI_STATUS_INVALID_ARGUMENT);
    policy.topP = 0.9F;
    policy.topK = 0U;
    CHECK(UmiAiTrainingGenerationPerformanceCompare(model, prompt, 3U,
        &policy, 2U, 99U, &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&report, savedReport, sizeof report) == 0);
    UmiAiTrainingDecoderDestroy(model);
    puts("native generation measurement invalid inputs and output isolation PASS");
    return 0;
}
