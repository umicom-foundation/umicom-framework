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

/*
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

/**
 * Represent the artifact index data shared with callers of this public contract.
 */
typedef struct UmiArtifactIndex {
    UmiDeliveryArtifact artifacts[UMI_DELIVERY_MAX_ARTIFACTS];
    size_t count;
} UmiArtifactIndex;
/**
 * Initialise artifact index from caller-provided values so later operations receive a
 * known state.
 */
void umi_artifact_index_init(UmiArtifactIndex *index);
/**
 * Add artifact index only after its inputs and available capacity have been checked.
 */
UmiStatus umi_artifact_index_add(UmiArtifactIndex *index,
                                 const UmiDeliveryArtifact *artifact);
/**
 * Provide the artifact index get operation used by this module and its client
 * applications.
 */
const UmiDeliveryArtifact *umi_artifact_index_get(const UmiArtifactIndex *index,
                                                   const char *artifact_id);

#ifdef __cplusplus
}
#endif

#endif
