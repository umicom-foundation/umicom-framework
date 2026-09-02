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

/*
 * This fixed-capacity collection avoids hidden allocation while release planning is still small and inspectable.
 */

#include "umicom/delivery/artifact_set.h"
#include <string.h>

/*
 * Initialise artifact set from caller-provided values so later operations receive a known
 * state.
 */
void umi_artifact_set_init(UmiArtifactSet *set)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set != NULL) (void)memset(set, 0, sizeof(*set));
}

/* Add artifact set only after its inputs and available capacity have been checked. */
UmiStatus umi_artifact_set_add(UmiArtifactSet *set,
                               const UmiDeliveryArtifact *artifact)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set == NULL || artifact == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (set->count >= UMI_DELIVERY_MAX_ARTIFACTS) return UMI_STATUS_CAPACITY_EXCEEDED;
    set->items[set->count++] = *artifact;
    return UMI_STATUS_OK;
}

/* Find artifact set while leaving the underlying catalogue or model owned by this module. */
const UmiDeliveryArtifact *umi_artifact_set_find(const UmiArtifactSet *set,
                                                  const char *artifact_id)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set == NULL || artifact_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < set->count; ++i) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(set->items[i].artifact_id, artifact_id) == 0) return &set->items[i];
    }
    return NULL;
}

/*
 * Return the number of records represented by artifact set total without changing their
 * state.
 */
uint64_t umi_artifact_set_total_size(const UmiArtifactSet *set)
{
    size_t i;
    uint64_t total = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set == NULL) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < set->count; ++i) total += set->items[i].size_bytes;
    return total;
}

/*
 * Provide the artifact set count kind operation used by this module and its client
 * applications.
 */
size_t umi_artifact_set_count_kind(const UmiArtifactSet *set,
                                   UmiArtifactKind kind)
{
    size_t i;
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set == NULL) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < set->count; ++i) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (set->items[i].kind == kind) ++count;
    }
    return count;
}
