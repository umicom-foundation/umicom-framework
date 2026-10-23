/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_dataset_split.c
 * Purpose: Prove seeded, disjoint sequence partitions and failure atomicity.
 * Author: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------
 * File: tests/ai_training/test_training_dataset_split.c
 * LICENCE: MIT
 */
#include "umicom/ai/training/dataset_split.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); return 1; } } while (0)
int main(void)
{
    size_t a[12], b[12], c[12], seen[12] = {0};
    for (size_t i = 0; i < 12; ++i) a[i] = (size_t)-1;
    CHECK(UmiAiTrainingSplitSequences(12, 3, 0xAFFA, a, 11) == UMI_STATUS_CAPACITY_EXCEEDED);
    for (size_t i = 0; i < 12; ++i) CHECK(a[i] == (size_t)-1);
    CHECK(UmiAiTrainingSplitSequences(12, 0, 6, a, 12) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingSplitSequences(12, 12, 6, a, 12) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingSplitSequences(12, 3, 6, a, 12) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingSplitSequences(12, 3, 6, b, 12) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingSplitSequences(12, 3, 7, c, 12) == UMI_STATUS_OK);
    CHECK(memcmp(a,b,sizeof(a)) == 0);
    size_t differences = 0U;
    for (size_t i = 0; i < 12; ++i) {
        CHECK(a[i] < 12 && seen[a[i]] == 0);
        seen[a[i]] = 1;
        if (a[i] != c[i]) differences++;
    }
    CHECK(differences > 0U);
    puts("Whole-sequence split: passed");
    return 0;
}
