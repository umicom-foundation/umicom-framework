/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_benchmark_rejections.c
 * PURPOSE: Reject invalid cases and insufficient output capacity without
 *          publishing partial results, even when a later case is malformed.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------
 * File: tests/ai_training/test_training_benchmark_rejections.c
 * LICENCE: MIT
 */
#include "umicom/ai/training/benchmark.h"
#include <stdio.h>
#include <string.h>
#define CHECK(test) do { if (!(test)) { \
    fprintf(stderr, "check failed at line %d: %s\n", __LINE__, #test); return 1; \
} } while (0)

int main(void)
{
    const UmiAiTrainingDecoderConfig config = {260U, 2U, 4U, 8U, 2U, 97U};
    const UmiAiTrainingDecoderConfig otherConfig = {12U, 2U, 4U, 8U, 2U, 97U};
    UmiAiTrainingDecoder *model = NULL, *other = NULL;
    UmiAiTrainingBenchmarkCaseResult rows[2], savedRows[2];
    UmiAiTrainingBenchmarkReport report, savedReport;
    UmiAiTrainingSamplingOptions options = {1.0F, 1U};
    const uint8_t prompt[] = {'X'}, expected[] = {'Y'};
    UmiAiTrainingBenchmarkCase cases[2] = {
        {"one", prompt, 1U, expected, 1U},
        {"two", prompt, 1U, expected, 1U}
    };
    UmiStatus status;
    CHECK(UmiAiTrainingDecoderCreate(&config, &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCreate(&otherConfig, &other) == UMI_STATUS_OK);
    memset(rows, 0x5A, sizeof(rows)); memset(&report, 0x66, sizeof(report));
    memcpy(savedRows, rows, sizeof(rows)); savedReport = report;
    #define PRESERVED() (memcmp(rows, savedRows, sizeof(rows)) == 0 && \
                         memcmp(&report, &savedReport, sizeof(report)) == 0)
    status = UmiAiTrainingBenchmarkRun(model, cases, 2U, &options, 4U,
                                      rows, 1U, &report);
    CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED && PRESERVED());
    cases[1].expectedLength = UMI_AI_GENERATION_MAX_NEW_TOKENS + 1U;
    status = UmiAiTrainingBenchmarkRun(model, cases, 2U, &options, 4U,
                                      rows, 2U, &report);
    CHECK(status == UMI_STATUS_INVALID_ARGUMENT && PRESERVED());
    cases[1].expectedLength = 1U;
    cases[1].expectedBytes = NULL;
    status = UmiAiTrainingBenchmarkRun(model, cases, 2U, &options, 4U,
                                      rows, 2U, &report);
    CHECK(status == UMI_STATUS_INVALID_ARGUMENT && PRESERVED());
    cases[1].expectedBytes = expected;
    options.temperature = 0.0F;
    status = UmiAiTrainingBenchmarkRun(model, cases, 2U, &options, 4U,
                                      rows, 2U, &report);
    CHECK(status == UMI_STATUS_INVALID_ARGUMENT && PRESERVED());
    options.temperature = 1.0F;
    status = UmiAiTrainingBenchmarkRun(other, cases, 2U, &options, 4U,
                                      rows, 2U, &report);
    CHECK(status == UMI_STATUS_INVALID_ARGUMENT && PRESERVED());
    status = UmiAiTrainingBenchmarkRun(model, cases, 0U, &options, 4U,
                                      rows, 2U, &report);
    CHECK(status == UMI_STATUS_INVALID_ARGUMENT && PRESERVED());
    UmiAiTrainingDecoderDestroy(other);
    UmiAiTrainingDecoderDestroy(model);
    return 0;
}
