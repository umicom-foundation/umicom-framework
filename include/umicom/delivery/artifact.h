/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/artifact.h
 *
 * PURPOSE:
 *   Represent one distributable file or metadata artifact and its verification state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * An artifact is one output of a release, such as an executable, ZIP file, installer, SBOM or checksum.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_ARTIFACT_H
#define INCLUDE_UMICOM_DELIVERY_ARTIFACT_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the delivery artifact data shared with callers of this public contract.
 */
typedef struct UmiDeliveryArtifact {
    char artifact_id[UMI_DELIVERY_ID_CAPACITY];
    UmiArtifactKind kind;
    char path[UMI_DELIVERY_PATH_CAPACITY];
    uint64_t size_bytes;
    char digest[UMI_DELIVERY_DIGEST_CAPACITY];
    int required;
    int verified;
} UmiDeliveryArtifact;

/**
 * Initialise delivery artifact from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_delivery_artifact_init(UmiDeliveryArtifact *artifact,
                                     const char *artifact_id,
                                     UmiArtifactKind kind,
                                     const char *path);
/**
 * Provide the delivery artifact set digest operation used by this module and its client
 * applications.
 */
UmiStatus umi_delivery_artifact_set_digest(UmiDeliveryArtifact *artifact,
                                           const char *digest);
/**
 * Provide the delivery artifact mark verified operation used by this module and its client
 * applications.
 */
void umi_delivery_artifact_mark_verified(UmiDeliveryArtifact *artifact,
                                         int verified);

#ifdef __cplusplus
}
#endif

#endif
