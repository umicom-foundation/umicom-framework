/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/artifact_index.h
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

/* BEGINNER NOTE:
 * An explicit index keeps release consumers from repeatedly scanning unrelated package metadata.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_ARTIFACT_INDEX_H
#define INCLUDE_UMICOM_DELIVERY_ARTIFACT_INDEX_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"
#include "umicom/delivery/artifact.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiArtifactIndex {
    UmiDeliveryArtifact artifacts[UMI_DELIVERY_MAX_ARTIFACTS];
    size_t count;
} UmiArtifactIndex;
void umi_artifact_index_init(UmiArtifactIndex *index);
UmiStatus umi_artifact_index_add(UmiArtifactIndex *index,
                                 const UmiDeliveryArtifact *artifact);
const UmiDeliveryArtifact *umi_artifact_index_get(const UmiArtifactIndex *index,
                                                   const char *artifact_id);

#ifdef __cplusplus
}
#endif

#endif
