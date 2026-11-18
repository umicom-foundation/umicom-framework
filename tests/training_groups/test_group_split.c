/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/training_groups/test_group_split.c
 *
 * PURPOSE:
 *   Check group isolation, row-order invariance, bounded inputs and unchanged output on failure.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/dataset_split.h"
#include <stdio.h>
#include <string.h>
#define REQUIRE(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); return 1; } } while (0)

int main(int argc, char **argv)
{
    REQUIRE(argc == 2);
    const char *mode = argv[1];
    uint64_t ids[8] = {40U, 10U, 40U, 20U, 10U, 30U, 20U, 30U};
    unsigned char result[10];
    memset(result, 0xa5, sizeof(result));
    if (strcmp(mode, "isolation") == 0 || strcmp(mode, "repeat") == 0 ||
        strcmp(mode, "reorder") == 0 || strcmp(mode, "extreme-ids") == 0) {
        if (strcmp(mode, "extreme-ids") == 0) {ids[0] = ids[2] = UINT64_MAX; ids[1] = ids[4] = 0U;}
        REQUIRE(UmiAiTrainingSplitGroups(ids, 8U, 2U, 17U, result, sizeof(result)) == UMI_STATUS_OK);
        size_t evaluation = 0U;
        for (size_t i = 0U; i < 8U; ++i) {
            REQUIRE(result[i] <= 1U); evaluation += result[i];
            for (size_t j = 0U; j < 8U; ++j)
                if (ids[i] == ids[j]) REQUIRE(result[i] == result[j]);
        }
        REQUIRE(evaluation == 4U && result[8] == 0xa5 && result[9] == 0xa5);
        unsigned char again[8];
        if (strcmp(mode, "reorder") == 0) {
            uint64_t reversed[8];
            for (size_t i = 0U; i < 8U; ++i) reversed[i] = ids[7U - i];
            REQUIRE(UmiAiTrainingSplitGroups(reversed, 8U, 2U, 17U, again, 8U) == UMI_STATUS_OK);
            for (size_t i = 0U; i < 8U; ++i) REQUIRE(again[i] == result[7U - i]);
        } else {
            REQUIRE(UmiAiTrainingSplitGroups(ids, 8U, 2U, 17U, again, 8U) == UMI_STATUS_OK);
            REQUIRE(memcmp(result, again, 8U) == 0);
        }
    } else if (strcmp(mode, "row-counts") == 0) {
        uint64_t expanded[] = {10U, 20U, 30U, 40U, 40U, 40U, 40U, 40U};
        unsigned char baseline[8];
        REQUIRE(UmiAiTrainingSplitGroups(ids, 8U, 2U, 0U, baseline, 8U) == UMI_STATUS_OK);
        REQUIRE(UmiAiTrainingSplitGroups(expanded, 8U, 2U, 0U, result, 8U) == UMI_STATUS_OK);
        for (size_t i = 0U; i < 8U; ++i)
            for (size_t j = 0U; j < 8U; ++j)
                if (expanded[i] == ids[j]) REQUIRE(result[i] == baseline[j]);
    } else if (strcmp(mode, "limit") == 0) {
        uint64_t many[UMI_AI_TRAINING_MAX_GROUP_ROWS];
        unsigned char chosen[UMI_AI_TRAINING_MAX_GROUP_ROWS];
        for (size_t i = 0U; i < UMI_AI_TRAINING_MAX_GROUP_ROWS; ++i) many[i] = (uint64_t)i;
        REQUIRE(UmiAiTrainingSplitGroups(many, UMI_AI_TRAINING_MAX_GROUP_ROWS, 100U, 5U,
                                         chosen, sizeof(chosen)) == UMI_STATUS_OK);
        size_t total = 0U;
        for (size_t i = 0U; i < sizeof(chosen); ++i) total += chosen[i];
        REQUIRE(total == 100U);
    } else {
        UmiStatus expected = UMI_STATUS_INVALID_ARGUMENT;
        size_t rows = 8U, groups = 2U, capacity = sizeof(result);
        const uint64_t *input = ids;
        unsigned char *output = result;
        if (strcmp(mode, "null-input") == 0) input = NULL;
        else if (strcmp(mode, "null-output") == 0) output = NULL;
        else if (strcmp(mode, "empty") == 0) rows = 0U;
        else if (strcmp(mode, "single-row") == 0) rows = 1U;
        else if (strcmp(mode, "over-limit") == 0) rows = UMI_AI_TRAINING_MAX_GROUP_ROWS + 1U;
        else if (strcmp(mode, "zero-evaluation") == 0) groups = 0U;
        else if (strcmp(mode, "all-groups") == 0) groups = 4U;
        else if (strcmp(mode, "one-group") == 0) {for (size_t i = 0U; i < 8U; ++i) ids[i] = 7U;}
        else if (strcmp(mode, "short-output") == 0) {capacity = 7U; expected = UMI_STATUS_CAPACITY_EXCEEDED;}
        else return 2;
        REQUIRE(UmiAiTrainingSplitGroups(input, rows, groups, 0U, output, capacity) == expected);
        for (size_t i = 0U; i < sizeof(result); ++i) REQUIRE(result[i] == 0xa5);
    }
    puts("Group split contract passed");
    return 0;
}
