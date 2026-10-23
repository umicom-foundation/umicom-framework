/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_kv_prefix_rejections.c
 * PURPOSE: Reject invalid inputs without altering public output objects.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/kv_prefix_reuse.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expr); \
    return 1; } } while (0)

int main(void)
{
    UmiAiTrainingDecoderConfig config = {
        260U, 8U, 8U, 16U, 2U, UINT64_C(304)
    };
    UmiAiTrainingDecoder *model = NULL;
    const size_t tokens[] = {4U, 5U, 6U};
    const size_t invalidTokens[] = {4U, 260U};
    UmiAiTrainingKvPrefixWorkEstimate estimate;
    UmiAiTrainingKvPrefixPerformance report;
    unsigned char savedEstimate[sizeof estimate];
    unsigned char savedReport[sizeof report];

    memset(&estimate, 0xA5, sizeof estimate);
    memcpy(savedEstimate, &estimate, sizeof estimate);
    CHECK(UmiAiTrainingKvPrefixWorkEstimateBuild(NULL, 2U,
        &estimate) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingKvPrefixWorkEstimateBuild(&config, 0U,
        &estimate) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingKvPrefixWorkEstimateBuild(&config, 9U,
        &estimate) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingKvPrefixWorkEstimateBuild(&config, 1U,
        NULL) == UMI_STATUS_INVALID_ARGUMENT);
    config.attentionHeads = 3U;
    CHECK(UmiAiTrainingKvPrefixWorkEstimateBuild(&config, 1U,
        &estimate) != UMI_STATUS_OK);
    CHECK(memcmp(&estimate, savedEstimate, sizeof estimate) == 0);
    config.attentionHeads = 2U;

    CHECK(UmiAiTrainingDecoderCreate(&config, &model) == UMI_STATUS_OK);
    memset(&report, 0x5A, sizeof report);
    memcpy(savedReport, &report, sizeof report);
    CHECK(UmiAiTrainingKvPrefixPerformanceCompare(NULL, tokens, 2U,
        &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingKvPrefixPerformanceCompare(model, NULL, 2U,
        &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingKvPrefixPerformanceCompare(model, tokens, 0U,
        &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingKvPrefixPerformanceCompare(model, tokens, 9U,
        &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingKvPrefixPerformanceCompare(model, invalidTokens, 2U,
        &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingKvPrefixPerformanceCompare(model, tokens, 2U,
        NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&report, savedReport, sizeof report) == 0);
    UmiAiTrainingDecoderDestroy(model);
    puts("KV growing-prefix reuse: rejection and failure-atomic reporting PASS");
    return 0;
}
