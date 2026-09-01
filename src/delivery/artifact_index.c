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
void umi_artifact_index_init(UmiArtifactIndex *index)
{
    if (index != NULL) (void)memset(index, 0, sizeof(*index));
}
UmiStatus umi_artifact_index_add(UmiArtifactIndex *index,
                                 const UmiDeliveryArtifact *artifact)
{
    if (index == NULL || artifact == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index->count >= UMI_DELIVERY_MAX_ARTIFACTS) return UMI_STATUS_CAPACITY_EXCEEDED;
    index->artifacts[index->count++] = *artifact;
    return UMI_STATUS_OK;
}
const UmiDeliveryArtifact *umi_artifact_index_get(const UmiArtifactIndex *index,
                                                   const char *artifact_id)
{
    size_t i;
    if (index == NULL || artifact_id == NULL) return NULL;
    for (i = 0U; i < index->count; ++i) {
        if (strcmp(index->artifacts[i].artifact_id, artifact_id) == 0) return &index->artifacts[i];
    }
    return NULL;
}
