/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_memory_budget.c
 * PURPOSE: Verify committed tape-memory budgets and unchanged graph outputs
 *          across rejection, rewind, rollback and resumed operations.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------
 * File: tests/ai_training/test_training_memory_budget.c
 * LICENCE: MIT
 */
#include "umicom/ai/training/tape.h"
#include <stdio.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #x); return 1; \
} } while (0)

int main(void)
{
    UmiAiTrainingTape *tape = NULL;
    UmiAiTrainingTensor *leaf = NULL;
    UmiAiTrainingTensor *result = NULL;
    UmiAiTrainingTensor *unchanged = NULL;
    UmiAiTrainingTapeMemory base = {0}, withLeaf = {0}, after = {0};
    UmiAiTrainingTapeMemory preserved;
    float values[4] = {1.0F, -2.0F, 3.0F, 4.0F};
    size_t checkpoint = 0U;

    CHECK(UmiAiTrainingTapeCreate(&tape) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTapeMemoryInspect(tape, &base) == UMI_STATUS_OK);
    CHECK(base.tensorCount == 0U && base.currentBytes > 0U &&
          base.currentBytes == base.peakBytes && base.budgetBytes == 0U);

    preserved = base;
    CHECK(UmiAiTrainingTapeMemoryInspect(NULL, &preserved) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&preserved, &base, sizeof(base)) == 0);
    CHECK(UmiAiTrainingTapeSetMemoryBudget(tape, base.currentBytes - 1U)
          == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(UmiAiTrainingTapeMemoryInspect(tape, &after) == UMI_STATUS_OK);
    CHECK(after.budgetBytes == 0U && after.currentBytes == base.currentBytes);

    CHECK(UmiAiTrainingTensorCreate(tape, 2U, 2U, values, 4U, 1, &leaf)
          == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTapeMemoryInspect(tape, &withLeaf) == UMI_STATUS_OK);
    CHECK(withLeaf.tensorCount == 1U &&
          withLeaf.currentBytes > base.currentBytes &&
          withLeaf.tensorValuesBytes == 4U * sizeof(float) &&
          withLeaf.tensorGradientBytes == 4U * sizeof(float));
    CHECK(UmiAiTrainingTapeSetMemoryBudget(tape, withLeaf.currentBytes + 1U)
          == UMI_STATUS_OK);
    unchanged = leaf;
    CHECK(UmiAiTrainingRelu(tape, leaf, &unchanged) ==
          UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(unchanged == leaf);
    CHECK(UmiAiTrainingTapeMemoryInspect(tape, &after) == UMI_STATUS_OK);
    CHECK(after.tensorCount == 1U &&
          after.currentBytes == withLeaf.currentBytes &&
          after.peakBytes == withLeaf.peakBytes &&
          after.budgetBytes == withLeaf.currentBytes + 1U);

    CHECK(UmiAiTrainingTapeSetMemoryBudget(tape, 0U) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingRelu(tape, leaf, &result) == UMI_STATUS_OK);
    CHECK(result != NULL && result != leaf);
    CHECK(UmiAiTrainingTapeMemoryInspect(tape, &after) == UMI_STATUS_OK);
    CHECK(after.tensorCount == 2U &&
          after.currentBytes > withLeaf.currentBytes &&
          after.peakBytes == after.currentBytes);
    CHECK(UmiAiTrainingTapeCheckpoint(tape, &checkpoint) == UMI_STATUS_OK);
    CHECK(checkpoint == 2U);
    CHECK(UmiAiTrainingRelu(tape, result, &unchanged) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTapeRollback(tape, checkpoint) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTapeMemoryInspect(tape, &after) == UMI_STATUS_OK);
    CHECK(after.tensorCount == 2U && after.peakBytes > after.currentBytes);

    CHECK(UmiAiTrainingTapeRewind(tape, 1U) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTapeMemoryInspect(tape, &after) == UMI_STATUS_OK);
    CHECK(after.tensorCount == 1U &&
          after.currentBytes == withLeaf.currentBytes &&
          after.peakBytes > after.currentBytes);
    CHECK(UmiAiTrainingTapeRewind(tape, 0U) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTapeMemoryInspect(tape, &after) == UMI_STATUS_OK);
    CHECK(after.tensorCount == 0U && after.currentBytes == base.currentBytes);
    UmiAiTrainingTapeDestroy(tape);
    puts("Committed memory budget, rollback and rewind: PASS");
    return 0;
}
