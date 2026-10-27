/*-----------------------------------------------------------------------------
 * Umicom Framework | Single-query forward-kernel numerical tests
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/single_query_attention.h"
#include "umicom/ai/training/tape.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define REQUIRE(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
    return 1; \
} } while (0)

static int Approximately(float actual, double expected, double tolerance)
{
    return fabs((double)actual - expected) <= tolerance;
}

static int OneHeadExample(void)
{
    const float q[2] = {1.0F, 0.0F};
    const float k[4] = {1.0F, 0.0F, 0.0F, 1.0F};
    const float v[4] = {2.0F, 4.0F, 6.0F, 8.0F};
    const double t = exp(1.0 / sqrt(2.0));
    const double p = t / (t + 1.0);
    float out[2] = {-99.0F, -99.0F};
    UmiAiTrainingSingleQueryAttentionReport report = {0};
    REQUIRE(UmiAiTrainingSingleQueryAttentionForward(q, 2U, k, 4U, v, 4U,
        2U, 2U, 1U, out, 2U, &report) == UMI_STATUS_OK);
    REQUIRE(Approximately(out[0], 2.0 * p + 6.0 * (1.0 - p), 2e-6));
    REQUIRE(Approximately(out[1], 4.0 * p + 8.0 * (1.0 - p), 2e-6));
    REQUIRE(report.validRows == 2U && report.channels == 2U);
    REQUIRE(report.scoredKeyRows == 2U && report.dotProductTerms == 4U);
    REQUIRE(report.weightedValueTerms == 4U);
    return 0;
}

static int TwoHeadExample(void)
{
    const float q[4] = {1.0F, 0.0F, 0.0F, 1.0F};
    const float k[8] = {
        1.0F, 0.0F, 0.0F, 1.0F,
        0.0F, 1.0F, 1.0F, 0.0F};
    const float v[8] = {
        2.0F, 3.0F, 7.0F, 11.0F,
        13.0F, 17.0F, 19.0F, 23.0F};
    const double e = exp(1.0 / sqrt(2.0));
    const double p = e / (e + 1.0);
    const double expected[4] = {
        2.0 * p + 13.0 * (1.0 - p),
        3.0 * p + 17.0 * (1.0 - p),
        7.0 * p + 19.0 * (1.0 - p),
        11.0 * p + 23.0 * (1.0 - p)};
    float output[4] = {0};
    UmiAiTrainingSingleQueryAttentionReport report = {0};
    REQUIRE(UmiAiTrainingSingleQueryAttentionForward(q, 4U, k, 8U, v, 8U,
        2U, 4U, 2U, output, 4U, &report) == UMI_STATUS_OK);
    for (size_t i = 0U; i < 4U; ++i)
        REQUIRE(Approximately(output[i], expected[i], 3e-6));
    REQUIRE(report.headCount == 2U && report.headWidth == 2U);
    REQUIRE(report.scoredKeyRows == 4U && report.dotProductTerms == 8U);
    return 0;
}

static int SingleRowAndExtremeScore(void)
{
    const float q[2] = {2.0F, -3.0F};
    const float k[2] = {3.0F, 4.0F};
    const float v[2] = {-2.0F, 500.0F};
    float output[2] = {0.0F, 0.0F};
    REQUIRE(UmiAiTrainingSingleQueryAttentionForward(q, 2U, k, 2U, v, 2U,
        1U, 2U, 2U, output, 2U, NULL) == UMI_STATUS_OK);
    REQUIRE(output[0] == v[0] && output[1] == v[1]);

    const float veryLargeQ[2] = {20000.0F, -20000.0F};
    const float twoKeys[4] = {1.0F, 0.0F, 0.0F, 1.0F};
    const float twoValues[4] = {42.0F, -7.0F, 0.0F, 2.0F};
    REQUIRE(UmiAiTrainingSingleQueryAttentionForward(veryLargeQ, 2U,
        twoKeys, 4U, twoValues, 4U, 2U, 2U, 1U, output, 2U, NULL) == UMI_STATUS_OK);
    REQUIRE(output[0] == 42.0F && output[1] == -7.0F);
    return 0;
}

static int AtomicRejections(void)
{
    const float q[2] = {1.0F, 0.0F};
    const float k[4] = {1.0F, 0.0F, 0.0F, 1.0F};
    const float v[4] = {2.0F, 4.0F, 6.0F, 8.0F};
    const float badQ[2] = {NAN, 0.0F};
    const float badK[4] = {1.0F, 0.0F, INFINITY, 0.0F};
    const float badV[4] = {2.0F, 4.0F, NAN, 8.0F};
    float output[2] = {-41.0F, -43.0F};
    UmiAiTrainingSingleQueryAttentionReport report;
    memset(&report, 0xA5, sizeof(report));
    UmiAiTrainingSingleQueryAttentionReport unchanged = report;
    REQUIRE(UmiAiTrainingSingleQueryAttentionForward(q, 2U, k, 4U, v, 4U,
        2U, 2U, 3U, output, 2U, &report) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(UmiAiTrainingSingleQueryAttentionForward(q, 2U, k, 4U, v, 4U,
        2U, 2U, 1U, output, 1U, &report) == UMI_STATUS_CAPACITY_EXCEEDED);
    REQUIRE(UmiAiTrainingSingleQueryAttentionForward(badQ, 2U, k, 4U, v, 4U,
        2U, 2U, 1U, output, 2U, &report) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(UmiAiTrainingSingleQueryAttentionForward(q, 2U, badK, 4U, v, 4U,
        2U, 2U, 1U, output, 2U, &report) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(UmiAiTrainingSingleQueryAttentionForward(q, 2U, k, 4U, badV, 4U,
        2U, 2U, 1U, output, 2U, &report) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(UmiAiTrainingSingleQueryAttentionForward(q, 2U, k, 4U, v, 4U,
        2U, 2U, 1U, (float *)k, 4U, &report) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(UmiAiTrainingSingleQueryAttentionForward(q, 2U, k, 4U, v, 4U,
        UMI_AI_TRAINING_MAX_ELEMENTS + 1U, 2U, 1U, output, 2U,
        &report) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(output[0] == -41.0F && output[1] == -43.0F);
    REQUIRE(memcmp(&report, &unchanged, sizeof(report)) == 0);
    return 0;
}

int main(void)
{
    if (OneHeadExample() != 0) return 1;
    if (TwoHeadExample() != 0) return 1;
    if (SingleRowAndExtremeScore() != 0) return 1;
    if (AtomicRejections() != 0) return 1;
    puts("single-query numerical and rejection checks passed");
    return 0;
}
