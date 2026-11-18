/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/training_order/test_order_failures.c
 *
 * PURPOSE:
 *   Inject allocation and RNG refusals into the real epoch-order implementation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/initialisation.h"
#include "umicom/ai/training/sample_order.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%d: %s\n",__LINE__,#x); return 1; } } while (0)
static size_t attempts, requested, live, draws, failDraw;
static int failAllocation, failSeed;
static void *TrackedMalloc(size_t size)
{
    ++attempts; requested = size;
    if (failAllocation) return NULL;
    void *p = malloc(size); if (p) ++live; return p;
}
static void TrackedFree(void *p) { if (p) { --live; free(p); } }
static UmiStatus CheckedSeed(UmiAiTrainingRandomState *state, uint64_t seed)
{
    if (failSeed) return UMI_STATUS_INVALID_STATE;
    return UmiAiTrainingRandomSeed(state, seed);
}
static UmiStatus CheckedIndex(UmiAiTrainingRandomState *state, uint64_t bound, uint64_t *out)
{
    ++draws;
    if (draws == failDraw) return UMI_STATUS_INVALID_STATE;
    return UmiAiTrainingRandomIndex(state, bound, out);
}
/* Compile the actual implementation with observed resource calls. The normal
 * training archive supplies the RNG; no permutation algorithm is replaced. */
#define malloc TrackedMalloc
#define free TrackedFree
#define UmiAiTrainingRandomSeed CheckedSeed
#define UmiAiTrainingRandomIndex CheckedIndex
#include "../../src/ai/training/sample_order.c"
#undef malloc
#undef free
#undef UmiAiTrainingRandomSeed
#undef UmiAiTrainingRandomIndex
int main(void)
{
    uint32_t out[12], before[12];
    memset(out, 0x53, sizeof out); memcpy(before, out, sizeof out);
    CHECK(UmiAiTrainingSampleOrder(0U, 1U, out, 12U) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingSampleOrder(12U, 1U, out, 11U) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(attempts == 0U && live == 0U);
    failAllocation = 1;
    CHECK(UmiAiTrainingSampleOrder(10U, 1U, out, 12U) == UMI_STATUS_OUT_OF_MEMORY);
    CHECK(attempts == 1U && requested == 10U * sizeof(uint32_t) && live == 0U);
    CHECK(!memcmp(out, before, sizeof out));
    failAllocation = 0; failSeed = 1;
    CHECK(UmiAiTrainingSampleOrder(10U, 1U, out, 12U) == UMI_STATUS_INVALID_STATE);
    CHECK(live == 0U && !memcmp(out, before, sizeof out));
    failSeed = 0;
    for (failDraw = 1U; failDraw <= 9U; ++failDraw) {
        draws = 0U;
        CHECK(UmiAiTrainingSampleOrder(10U, 1U, out, 12U) == UMI_STATUS_INVALID_STATE);
        CHECK(live == 0U && !memcmp(out, before, sizeof out));
    }
    failDraw = 0U; draws = 0U; attempts = 0U;
    CHECK(UmiAiTrainingSampleOrder(10U, 1U, out, 12U) == UMI_STATUS_OK);
    CHECK(attempts == 1U && live == 0U && draws == 9U);
    CHECK(out[10] == before[10] && out[11] == before[11]);
    return 0;
}
