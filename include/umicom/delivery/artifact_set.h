/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/artifact_set.h
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

#ifndef INCLUDE_UMICOM_DELIVERY_ARTIFACT_SET_H
#define INCLUDE_UMICOM_DELIVERY_ARTIFACT_SET_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"
#include "umicom/delivery/artifact.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the artifact set data shared with callers of this public contract.
 */
typedef struct UmiArtifactSet {
    UmiDeliveryArtifact items[UMI_DELIVERY_MAX_ARTIFACTS];
    size_t count;
} UmiArtifactSet;

/**
 * Initialise artifact set from caller-provided values so later operations receive a known
 * state.
 */
void umi_artifact_set_init(UmiArtifactSet *set);
/**
 * Add artifact set only after its inputs and available capacity have been checked.
 */
UmiStatus umi_artifact_set_add(UmiArtifactSet *set,
                               const UmiDeliveryArtifact *artifact);
/**
 * Find artifact set while leaving the underlying catalogue or model owned by this module.
 */
const UmiDeliveryArtifact *umi_artifact_set_find(const UmiArtifactSet *set,
                                                  const char *artifact_id);
/**
 * Return the number of records represented by artifact set total without changing their
 * state.
 */
uint64_t umi_artifact_set_total_size(const UmiArtifactSet *set);
/**
 * Provide the artifact set count kind operation used by this module and its client
 * applications.
 */
size_t umi_artifact_set_count_kind(const UmiArtifactSet *set,
                                   UmiArtifactKind kind);

#ifdef __cplusplus
}
#endif

#endif
