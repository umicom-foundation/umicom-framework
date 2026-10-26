/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_stacked_prefix_boundaries.c
 * PURPOSE: Ensure rejected pulls and stale borrowed models cannot publish
 *          a partial result or modify an established model checkpoint.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation | LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/stacked_prefix_inference.h"
#include "umicom/ai/training/stacked_prefix_qualification.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "boundaries line %d: %s\n", __LINE__, #x); return 1; } } while (0)
int main(void)
{
    const UmiAiTrainingStackedDecoderConfig config = {32U, 4U, 4U, 8U, 2U, 2U, UINT64_C(983)};
    const size_t prompt[] = {5U, 9U};
    const UmiAiTrainingNucleusOptions greedy = {1.0F, 1U, 1.0F};
    UmiAiTrainingStackedDecoder *model = NULL;
    UmiAiTrainingStackedPrefixSession *session = NULL;
    UmiAiTrainingStackedPrefixStats report = {0};
    size_t output[2] = {999U, 999U};
    size_t count = 444U, next = 999U;
    CHECK(UmiAiTrainingStackedDecoderCreate(&config, &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedPrefixGenerateNucleus(model, prompt, 2U,
          &greedy, 2U, UINT64_C(9), output, 1U, &count, &report) ==
          UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(output[0] == 999U && output[1] == 999U && count == 444U);
    CHECK(UmiAiTrainingStackedPrefixSessionCreate(model, prompt, 2U,
          &greedy, UINT64_C(9), &session) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedPrefixSessionNext(session, NULL) ==
          UMI_STATUS_INVALID_ARGUMENT);
    uint8_t byteSentinel = 99U;
    CHECK(UmiAiTrainingStackedPrefixSessionNextByte(session, &byteSentinel) ==
          UMI_STATUS_INVALID_ARGUMENT); /* Not the exact byte vocabulary. */
    CHECK(byteSentinel == 99U);
    CHECK(UmiAiTrainingStackedPrefixSessionInspect(session, &report) == UMI_STATUS_OK);
    CHECK(report.generatedTokens == 0U);
    CHECK(UmiAiTrainingStackedPrefixSessionNext(session, &next) == UMI_STATUS_OK);
    CHECK(next != 999U);
    CHECK(UmiAiTrainingStackedPrefixSessionInspect(session, &report) == UMI_STATUS_OK);
    CHECK(report.generatedTokens == 1U);
    /* A real training operation changes the borrowed decoder checkpoint.
     * The existing streaming session must then reject continued use. */
    const size_t inputs[4] = {5U, 9U, 7U, 8U};
    const size_t targets[4] = {6U, 10U, 8U, 9U};
    const uint8_t mask[4] = {0U, 1U, 1U, 0U};
    UmiAiTrainingStackedStep changed = {0};
    CHECK(UmiAiTrainingStackedDecoderTrainSelected(model, inputs, targets,
          mask, 4U, 0.001F, &changed) == UMI_STATUS_OK);
    next = 999U;
    CHECK(UmiAiTrainingStackedPrefixSessionNext(session, &next) ==
          UMI_STATUS_INVALID_STATE);
    CHECK(next == 999U);
    UmiAiTrainingStackedPrefixSessionDestroy(session);
    UmiAiTrainingStackedDecoderDestroy(model);
    puts("stacked prefix: rejected calls and changed model identity preserved");
    return 0;
}
