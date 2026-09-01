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

typedef struct UmiArtifactSet {
    UmiDeliveryArtifact items[UMI_DELIVERY_MAX_ARTIFACTS];
    size_t count;
} UmiArtifactSet;

void umi_artifact_set_init(UmiArtifactSet *set);
UmiStatus umi_artifact_set_add(UmiArtifactSet *set,
                               const UmiDeliveryArtifact *artifact);
const UmiDeliveryArtifact *umi_artifact_set_find(const UmiArtifactSet *set,
                                                  const char *artifact_id);
uint64_t umi_artifact_set_total_size(const UmiArtifactSet *set);
size_t umi_artifact_set_count_kind(const UmiArtifactSet *set,
                                   UmiArtifactKind kind);

#ifdef __cplusplus
}
#endif

#endif
