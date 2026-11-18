/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/dataset_group_split.c
 *
 * PURPOSE:
 *   Keep caller-identified source groups intact across reproducible training and evaluation partitions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/dataset_split.h"
#include "umicom/ai/training/initialisation.h"
#include <stdlib.h>
#include <string.h>

static int CompareGroupId(const void *left, const void *right)
{
    uint64_t a = *(const uint64_t *)left, b = *(const uint64_t *)right;
    /* Subtraction would wrap for wide IDs and violate qsort's ordering. */
    return (a > b) - (a < b);
}

static size_t FindGroupId(const uint64_t *ids, size_t count, uint64_t id)
{
    size_t first = 0U, last = count;
    while (first < last) {
        size_t middle = first + (last - first) / 2U;
        if (ids[middle] < id) first = middle + 1U;
        else last = middle;
    }
    return first;
}

UmiStatus UmiAiTrainingSplitGroups(const uint64_t *groupIds,
    size_t rowCount, size_t evaluationGroupCount, uint64_t seed,
    unsigned char *outEvaluation, size_t capacity)
{
    if (groupIds == NULL || outEvaluation == NULL || rowCount < 2U ||
        rowCount > UMI_AI_TRAINING_MAX_GROUP_ROWS || evaluationGroupCount == 0U ||
        evaluationGroupCount >= rowCount)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (capacity < rowCount) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* The validated row cap bounds every allocation product. Output remains
     * transactional: copy candidate bytes only after all groups are mapped. */
    uint64_t *ids = malloc(rowCount * sizeof(*ids));
    size_t *order = malloc(rowCount * sizeof(*order));
    unsigned char *membership = calloc(rowCount, sizeof(*membership));
    unsigned char *candidate = malloc(rowCount);
    UmiStatus status = UMI_STATUS_OUT_OF_MEMORY;
    if (ids == NULL || order == NULL || membership == NULL || candidate == NULL)
        goto finished;
    memcpy(ids, groupIds, rowCount * sizeof(*ids));
    qsort(ids, rowCount, sizeof(*ids), CompareGroupId);
    size_t groups = 0U;
    for (size_t i = 0U; i < rowCount; ++i)
        if (groups == 0U || ids[i] != ids[groups - 1U]) ids[groups++] = ids[i];
    if (evaluationGroupCount >= groups) {
        status = UMI_STATUS_INVALID_ARGUMENT;
        goto finished;
    }
    for (size_t i = 0U; i < groups; ++i) order[i] = i;
    UmiAiTrainingRandomState random;
    status = UmiAiTrainingRandomSeed(&random, seed);
    for (size_t remaining = groups; status == UMI_STATUS_OK && remaining > 1U; --remaining) {
        uint64_t selected = 0U;
        status = UmiAiTrainingRandomIndex(&random, (uint64_t)remaining, &selected);
        if (status != UMI_STATUS_OK) goto finished;
        size_t swap = order[(size_t)selected];
        order[(size_t)selected] = order[remaining - 1U];
        order[remaining - 1U] = swap;
    }
    if (status != UMI_STATUS_OK) goto finished;
    for (size_t i = 0U; i < evaluationGroupCount; ++i) membership[order[i]] = 1U;
    for (size_t i = 0U; i < rowCount; ++i) {
        size_t group = FindGroupId(ids, groups, groupIds[i]);
        if (group == groups || ids[group] != groupIds[i]) {
            status = UMI_STATUS_INVALID_STATE;
            goto finished;
        }
        candidate[i] = membership[group];
    }
    memcpy(outEvaluation, candidate, rowCount);
finished:
    free(candidate);
    free(membership);
    free(order);
    free(ids);
    return status;
}
