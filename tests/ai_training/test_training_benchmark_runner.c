/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_benchmark_runner.c
 * PURPOSE: Verify the benchmark delegates to the canonical byte sampler,
 *          remains reproducible and leaves model checkpoints unchanged.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/benchmark.h"
#include "umicom/ai/training/checkpoint.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(test) do { if (!(test)) { \
    fprintf(stderr, "check failed at line %d: %s\n", __LINE__, #test); return 1; \
} } while (0)

int main(void)
{
    const UmiAiTrainingDecoderConfig config = {260U, 3U, 4U, 8U, 2U, 42U};
    UmiAiTrainingDecoder *model = NULL, *restored = NULL;
    const UmiAiTrainingSamplingOptions options = {1.0F, 1U};
    const uint8_t p1[] = {'a'};
    const uint8_t p2[] = {'i', 'n', 't'};
    uint8_t expected1[5], expected2[6];
    size_t wrote1 = 0U, wrote2 = 0U;
    UmiAiTrainingBenchmarkCase tests[2];
    UmiAiTrainingBenchmarkCaseResult rows[2], replay[2];
    UmiAiTrainingBenchmarkReport report, again;
    uint8_t *before = NULL, *after = NULL;
    size_t required = 0U, firstLength = 0U, secondLength = 0U;

    CHECK(UmiAiTrainingDecoderCreate(&config, &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderGenerateBytes(model, p1, sizeof(p1), &options,
          sizeof(expected1), 81U, expected1, sizeof(expected1), &wrote1)
          == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderGenerateBytes(model, p2, sizeof(p2), &options,
          sizeof(expected2), 82U, expected2, sizeof(expected2), &wrote2)
          == UMI_STATUS_OK);
    CHECK(wrote1 == sizeof(expected1) && wrote2 == sizeof(expected2));
    tests[0] = (UmiAiTrainingBenchmarkCase){"account example", p1, sizeof(p1),
                                             expected1, sizeof(expected1)};
    tests[1] = (UmiAiTrainingBenchmarkCase){"C23 declaration", p2, sizeof(p2),
                                             expected2, sizeof(expected2)};

    CHECK(UmiAiTrainingDecoderCheckpointSize(model, &required) == UMI_STATUS_OK);
    CHECK(required > 0U);
    before = malloc(required); after = malloc(required);
    CHECK(before != NULL && after != NULL);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(model, before, required,
          &firstLength) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingBenchmarkRun(model, tests, 2U, &options, 81U,
          rows, 2U, &report) == UMI_STATUS_OK);
    CHECK(report.caseCount == 2U && report.exactMatchCases == 2U &&
          report.expectedBytes == 11U && report.generatedBytes == 11U &&
          report.matchingPositionBytes == 11U &&
          report.matchingPrefixBytes == 11U && report.completedOptimiserSteps == 0U);
    CHECK(rows[0].exactMatch && rows[1].exactMatch);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(model, after, required,
          &secondLength) == UMI_STATUS_OK);
    CHECK(firstLength == secondLength && memcmp(before, after, required) == 0);
    CHECK(UmiAiTrainingDecoderCheckpointDecode(before, required, &restored)
          == UMI_STATUS_OK);
    CHECK(UmiAiTrainingBenchmarkRun(restored, tests, 2U, &options, 81U,
          replay, 2U, &again) == UMI_STATUS_OK);
    CHECK(memcmp(rows, replay, sizeof(rows)) == 0);
    CHECK(memcmp(&report, &again, sizeof(report)) == 0);
    UmiAiTrainingDecoderDestroy(restored);
    UmiAiTrainingDecoderDestroy(model);
    free(after); free(before);
    return 0;
}
