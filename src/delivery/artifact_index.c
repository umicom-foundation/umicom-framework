/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/artifact_index.c
 *
 * PURPOSE:
 *   Index artifacts by identifier for package verification and Studio inspection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * An explicit index keeps release consumers from repeatedly scanning unrelated package metadata.
 */

#include "umicom/delivery/artifact_index.h"
#include <string.h>
/*
 * Initialise artifact index from caller-provided values so later operations receive a
 * known state.
 */
void umi_artifact_index_init(UmiArtifactIndex *index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (index != NULL) (void)memset(index, 0, sizeof(*index));
}
/* Add artifact index only after its inputs and available capacity have been checked. */
UmiStatus umi_artifact_index_add(UmiArtifactIndex *index,
                                 const UmiDeliveryArtifact *artifact)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (index == NULL || artifact == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index->count >= UMI_DELIVERY_MAX_ARTIFACTS) return UMI_STATUS_CAPACITY_EXCEEDED;
    index->artifacts[index->count++] = *artifact;
    return UMI_STATUS_OK;
}
/*
 * Provide the artifact index get operation used by this module and its client
 * applications.
 */
const UmiDeliveryArtifact *umi_artifact_index_get(const UmiArtifactIndex *index,
                                                   const char *artifact_id)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (index == NULL || artifact_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < index->count; ++i) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(index->artifacts[i].artifact_id, artifact_id) == 0) return &index->artifacts[i];
    }
    return NULL;
}
