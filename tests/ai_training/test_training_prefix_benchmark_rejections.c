/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_prefix_benchmark_rejections.c
 * PURPOSE:
 *   Reject invalid benchmark plans, prompts and sampling policies without
 *   mutating the caller's report or the decoder's portable checkpoint bytes.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/prefix_benchmark.h"
#include "umicom/ai/training/byte_tokenizer.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define CHECK(expr) do { if (!(expr)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; \
} } while (0)

int main(void)
{
    const UmiAiTrainingDecoderConfig config = {
        UMI_AI_BYTE_VOCABULARY, 8U, 8U, 16U, 2U, UINT64_C(81)
    };
    const size_t prompt[] = {109U, 114U, 120U};
    const size_t badPrompt[] = {UMI_AI_BYTE_VOCABULARY};
    UmiAiTrainingDecoder *model = NULL;
    UmiAiTrainingNucleusOptions policy = {0.85F, 12U, 0.9F};
    UmiAiTrainingPrefixBenchmarkPlan plan = {12U, 1U, 3U, 123U};
    UmiAiTrainingPrefixBenchmarkResult report;
    unsigned char saved[sizeof report];
    memset(&report, 0xA5, sizeof report);
    memcpy(saved, &report, sizeof report);
    CHECK(UmiAiTrainingDecoderCreate(&config, &model) == UMI_STATUS_OK);

    CHECK(UmiAiTrainingPrefixBenchmarkRun(NULL, prompt, 3U,
          &policy, &plan, &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingPrefixBenchmarkRun(model, NULL, 3U,
          &policy, &plan, &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingPrefixBenchmarkRun(model, prompt, 3U,
          NULL, &plan, &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingPrefixBenchmarkRun(model, prompt, 3U,
          &policy, NULL, &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingPrefixBenchmarkRun(model, prompt, 3U,
          &policy, &plan, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingPrefixBenchmarkRun(model, prompt, 0U,
          &policy, &plan, &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingPrefixBenchmarkRun(model, prompt, 9U,
          &policy, &plan, &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingPrefixBenchmarkRun(model, badPrompt, 1U,
          &policy, &plan, &report) == UMI_STATUS_INVALID_ARGUMENT);

    plan.generatedTokens = 0U;
    CHECK(UmiAiTrainingPrefixBenchmarkRun(model, prompt, 3U,
          &policy, &plan, &report) == UMI_STATUS_INVALID_ARGUMENT);
    plan.generatedTokens = UMI_AI_PREFIX_BENCHMARK_MAX_TOKENS + 1U;
    CHECK(UmiAiTrainingPrefixBenchmarkRun(model, prompt, 3U,
          &policy, &plan, &report) == UMI_STATUS_INVALID_ARGUMENT);
    plan.generatedTokens = 12U;
    plan.warmupPairs = UMI_AI_PREFIX_BENCHMARK_MAX_WARMUPS + 1U;
    CHECK(UmiAiTrainingPrefixBenchmarkRun(model, prompt, 3U,
          &policy, &plan, &report) == UMI_STATUS_INVALID_ARGUMENT);
    plan.warmupPairs = 1U;
    plan.measuredPairs = 0U;
    CHECK(UmiAiTrainingPrefixBenchmarkRun(model, prompt, 3U,
          &policy, &plan, &report) == UMI_STATUS_INVALID_ARGUMENT);
    plan.measuredPairs = UMI_AI_PREFIX_BENCHMARK_MAX_PAIRS + 1U;
    CHECK(UmiAiTrainingPrefixBenchmarkRun(model, prompt, 3U,
          &policy, &plan, &report) == UMI_STATUS_INVALID_ARGUMENT);
    plan.measuredPairs = 3U;
    policy.topP = 0.0F;
    CHECK(UmiAiTrainingPrefixBenchmarkRun(model, prompt, 3U,
          &policy, &plan, &report) == UMI_STATUS_INVALID_ARGUMENT);
    policy.topP = 0.9F;
    policy.topK = 0U;
    CHECK(UmiAiTrainingPrefixBenchmarkRun(model, prompt, 3U,
          &policy, &plan, &report) == UMI_STATUS_INVALID_ARGUMENT);
    policy.topK = 12U;
    policy.temperature = NAN;
    CHECK(UmiAiTrainingPrefixBenchmarkRun(model, prompt, 3U,
          &policy, &plan, &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&report, saved, sizeof report) == 0);

    UmiAiTrainingDecoderDestroy(model);
    puts("prefix benchmark: rejected plans preserve all caller-owned report bytes PASS");
    return 0;
}
