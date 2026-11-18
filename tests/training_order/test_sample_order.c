/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/training_order/test_sample_order.c
 *
 * PURPOSE:
 *   Check complete epoch membership, reproducibility and refusal atomicity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/sample_order.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%d: %s\n",__LINE__,#x); return 1; } } while (0)
static uint32_t first[UMI_AI_TRAINING_MAX_ORDER_ROWS + 2U];
static uint32_t second[UMI_AI_TRAINING_MAX_ORDER_ROWS + 2U];
static unsigned char seen[UMI_AI_TRAINING_MAX_ORDER_ROWS];
static int Verify(size_t count, uint64_t seed)
{
    memset(first, 0xA5, sizeof first);
    CHECK(UmiAiTrainingSampleOrder(count, seed, first, count + 2U) == UMI_STATUS_OK);
    memset(seen, 0, count);
    for (size_t i = 0U; i < count; ++i) {
        CHECK(first[i] < count && seen[first[i]] == 0U);
        seen[first[i]] = 1U;
    }
    for (size_t i = 0U; i < count; ++i) CHECK(seen[i] == 1U);
    CHECK(first[count] == UINT32_C(0xA5A5A5A5) &&
        first[count + 1U] == UINT32_C(0xA5A5A5A5));
    return 0;
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *name = argv[1];
    if (!strcmp(name, "vectors")) {
        const uint32_t expected[3][10] = {
            {6,3,2,9,8,1,4,7,0,5}, {4,2,8,1,9,3,0,6,7,5},
            {3,4,2,7,5,0,8,1,9,6}
        };
        const uint64_t seeds[3] = {0U, 1U, UINT64_MAX};
        /* Fixed vectors were independently calculated with 64-bit integer
         * arithmetic. Identity/reverse-only implementations cannot pass. */
        for (size_t i = 0U; i < 3U; ++i) {
            CHECK(UmiAiTrainingSampleOrder(10U, seeds[i], first, 10U) == UMI_STATUS_OK);
            CHECK(!memcmp(first, expected[i], sizeof expected[i]));
        }
        return 0;
    }
    if (!strcmp(name, "permutation")) {
        for (size_t n = 1U; n <= 257U; ++n)
            for (uint64_t seed = 0U; seed < 17U; ++seed) CHECK(Verify(n, seed) == 0);
        return 0;
    }
    if (!strcmp(name, "maximum")) return Verify(UMI_AI_TRAINING_MAX_ORDER_ROWS, UINT64_MAX);
    if (!strcmp(name, "repeat")) {
        CHECK(Verify(999U, UINT64_C(0xFEDCBA9876543210)) == 0);
        CHECK(UmiAiTrainingSampleOrder(999U, UINT64_C(0xFEDCBA9876543210), second, 999U) == UMI_STATUS_OK);
        CHECK(memcmp(first, second, 999U * sizeof(*first)) == 0);
        /* A different seed need not always differ, so only assert membership. */
        CHECK(Verify(999U, 1U) == 0);
        return 0;
    }
    if (!strcmp(name, "single")) {
        CHECK(Verify(1U, 0U) == 0 && first[0] == 0U);
        CHECK(Verify(1U, UINT64_MAX) == 0 && first[0] == 0U);
        return 0;
    }
    if (!strcmp(name, "batch-tail")) {
        CHECK(Verify(11U, 20U) == 0);
        memset(seen, 0, sizeof seen);
        size_t batches = 0U, visited = 0U, tail = 0U;
        for (size_t start = 0U; start < 11U;) {
            size_t take = 11U - start < 4U ? 11U - start : 4U;
            for (size_t j = 0U; j < take; ++j) {
                CHECK(!seen[first[start + j]]);
                seen[first[start + j]] = 1U; ++visited;
            }
            ++batches; tail = take; start += take;
        }
        CHECK(batches == 3U && tail == 3U && visited == 11U);
        return 0;
    }
    if (!strcmp(name, "arguments")) {
        memset(first, 0x79, sizeof first); memcpy(second, first, sizeof first);
        CHECK(UmiAiTrainingSampleOrder(0U, 0U, first, 1U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingSampleOrder(1U, 0U, NULL, 1U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingSampleOrder(UMI_AI_TRAINING_MAX_ORDER_ROWS + 1U, 0U, first, SIZE_MAX) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingSampleOrder(SIZE_MAX, 0U, first, SIZE_MAX) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(first, second, sizeof first) == 0);
        return 0;
    }
    if (!strcmp(name, "capacity")) {
        memset(first, 0x37, sizeof first); memcpy(second, first, sizeof first);
        CHECK(UmiAiTrainingSampleOrder(1U, 0U, first, 0U) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiAiTrainingSampleOrder(100U, 0U, first, 99U) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(first, second, sizeof first) == 0);
        CHECK(UmiAiTrainingSampleOrder(100U, 0U, first, 100U) == UMI_STATUS_OK);
        CHECK(first[100] == UINT32_C(0x37373737));
        return 0;
    }
    CHECK(0 && "unknown case");
    return 1;
}
