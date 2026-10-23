/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_sampling.c
 * PURPOSE: Stable top-K ordering, seeded sampling and failure-atomic outputs.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------
 * File: tests/ai_training/test_training_sampling.c
 * LICENCE: MIT
 */
#include "umicom/ai/training/generation.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #condition); \
    return 1; } } while (0)

int main(void)
{
    float logits[8] = {99.0F, 88.0F, 77.0F, 66.0F,
                       1.0F, 4.0F, 4.0F, -1.0F};
    UmiAiTrainingSamplingOptions options = {1.0F, 1U};
    UmiAiTrainingRandomState first, second;
    size_t token = 999U;
    CHECK(UmiAiTrainingRandomSeed(&first, UINT64_C(12345)) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingRandomSeed(&second, UINT64_C(12345)) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingSampleLogits(logits, 8U, 4U, 8U,
           &options, NULL, &token) == UMI_STATUS_OK && token == 5U);
    CHECK(UmiAiTrainingSampleLogits(logits, 8U, 5U, 8U,
           &options, &first, &token) == UMI_STATUS_OK && token == 5U);
    CHECK(first.state == UINT64_C(12345)); /* Greedy consumes no entropy. */

    options.topK = 2U;
    for (size_t count = 0U; count < 100U; ++count) {
        size_t tokenA = 999U, tokenB = 999U;
        CHECK(UmiAiTrainingSampleLogits(logits, 8U, 4U, 8U,
                  &options, &first, &tokenA) == UMI_STATUS_OK);
        CHECK(UmiAiTrainingSampleLogits(logits, 8U, 4U, 8U,
                  &options, &second, &tokenB) == UMI_STATUS_OK);
        CHECK(tokenA == tokenB && (tokenA == 5U || tokenA == 6U));
    }
    CHECK(first.state == second.state);

    uint64_t previous = first.state;
    options.topK = 0U;
    CHECK(UmiAiTrainingSampleLogits(logits, 8U, 4U, 8U,
            &options, &first, &token) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(first.state == previous && token != 999U); /* earlier success survives */
    token = 888U;
    options.topK = 5U;
    CHECK(UmiAiTrainingSampleLogits(logits, 8U, 4U, 8U,
            &options, &first, &token) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(token == 888U && first.state == previous);
    options.topK = 2U;
    options.temperature = 0.0F;
    CHECK(UmiAiTrainingSampleLogits(logits, 8U, 4U, 8U,
            &options, &first, &token) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(token == 888U && first.state == previous);
    options.temperature = 1.0F;
    logits[7] = NAN;
    CHECK(UmiAiTrainingSampleLogits(logits, 8U, 4U, 8U,
            &options, &first, &token) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(token == 888U && first.state == previous);
    logits[7] = -1.0F;
    logits[5] = 1.0e30F;
    logits[6] = -1.0e30F;
    options.temperature = 1.0e-20F;
    CHECK(UmiAiTrainingSampleLogits(logits, 8U, 4U, 8U,
            &options, &first, &token) == UMI_STATUS_OK);
    CHECK(token == 5U);
    puts("stable top-K, reproducible choices and invalid input: PASS");
    return 0;
}
