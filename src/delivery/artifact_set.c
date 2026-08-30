/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/artifact_set.c
 *
 * PURPOSE:
 *   Collect bounded release artifacts and provide lookup and size summaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This fixed-capacity collection avoids hidden allocation while release planning is still small and inspectable.
 */

#include "umicom/delivery/artifact_set.h"
#include <string.h>

void umi_artifact_set_init(UmiArtifactSet *set)
{
    if (set != NULL) (void)memset(set, 0, sizeof(*set));
}

UmiStatus umi_artifact_set_add(UmiArtifactSet *set,
                               const UmiDeliveryArtifact *artifact)
{
    if (set == NULL || artifact == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (set->count >= UMI_DELIVERY_MAX_ARTIFACTS) return UMI_STATUS_CAPACITY_EXCEEDED;
    set->items[set->count++] = *artifact;
    return UMI_STATUS_OK;
}

const UmiDeliveryArtifact *umi_artifact_set_find(const UmiArtifactSet *set,
                                                  const char *artifact_id)
{
    size_t i;
    if (set == NULL || artifact_id == NULL) return NULL;
    for (i = 0U; i < set->count; ++i) {
        if (strcmp(set->items[i].artifact_id, artifact_id) == 0) return &set->items[i];
    }
    return NULL;
}

uint64_t umi_artifact_set_total_size(const UmiArtifactSet *set)
{
    size_t i;
    uint64_t total = 0U;
    if (set == NULL) return 0U;
    for (i = 0U; i < set->count; ++i) total += set->items[i].size_bytes;
    return total;
}

size_t umi_artifact_set_count_kind(const UmiArtifactSet *set,
                                   UmiArtifactKind kind)
{
    size_t i;
    size_t count = 0U;
    if (set == NULL) return 0U;
    for (i = 0U; i < set->count; ++i) {
        if (set->items[i].kind == kind) ++count;
    }
    return count;
}
